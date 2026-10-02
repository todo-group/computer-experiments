#!/usr/bin/env node
// Print only after Reveal.js has built its PDF pages and fonts/math are ready.
import { spawn } from 'node:child_process';
import { mkdtemp, writeFile, rename, rm } from 'node:fs/promises';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';
import { pathToFileURL } from 'node:url';

const [chrome, input, output, timeoutSeconds = '120'] = process.argv.slice(2);
if (!chrome || !input || !output) {
  console.error('Usage: print_reveal_pdf.mjs CHROME INPUT.html OUTPUT.pdf [TIMEOUT_SECONDS]');
  process.exit(1);
}
const profile = await mkdtemp(join(tmpdir(), 'quarto-pdf-'));
const child = spawn(chrome, [
  '--headless', '--disable-gpu', '--no-first-run', '--no-default-browser-check',
  '--disable-background-networking', '--disable-component-update',
  `--user-data-dir=${profile}`, '--remote-debugging-pipe',
], { stdio: ['ignore', 'ignore', 'pipe', 'pipe', 'pipe'] });
let logs = '';
child.stderr.on('data', data => { logs = (logs + data).slice(-65536); });
let nextId = 0;
let buffer = '';
const pending = new Map();
function request(method, params = {}, sessionId) {
  return new Promise((resolveRequest, reject) => {
    const id = ++nextId;
    pending.set(id, { resolve: resolveRequest, reject });
    child.stdio[3].write(JSON.stringify({ id, method, params, sessionId }) + '\0');
  });
}
function rejectPending(error) {
  for (const item of pending.values()) item.reject(error);
  pending.clear();
}
child.on('error', rejectPending);
child.on('exit', () => rejectPending(new Error('Chrome exited before printing completed')));
child.stdio[3].on('error', rejectPending);
child.stdio[4].setEncoding('utf8');
child.stdio[4].on('data', data => {
  buffer += data;
  let boundary;
  while ((boundary = buffer.indexOf('\0')) !== -1) {
    const message = JSON.parse(buffer.slice(0, boundary));
    buffer = buffer.slice(boundary + 1);
    const item = pending.get(message.id);
    if (!item) continue;
    pending.delete(message.id);
    if (message.error) item.reject(new Error(message.error.message));
    else item.resolve(message.result);
  }
});

const timeoutMs = Number(timeoutSeconds) * 1000;
let timer;
let cancelled = false;
try {
  const timeout = new Promise((_, reject) => {
    timer = setTimeout(() => reject(new Error(`PDF generation timed out after ${timeoutSeconds}s`)), timeoutMs);
  });
  await Promise.race([timeout, (async () => {
    const { targetId } = await request('Target.createTarget', { url: 'about:blank' });
    const { sessionId } = await request('Target.attachToTarget', { targetId, flatten: true });
    const call = (method, params) => request(method, params, sessionId);
    await call('Page.enable');
    const url = pathToFileURL(resolve(input));
    url.search = 'print-pdf';
    const navigation = await call('Page.navigate', { url: url.href });
    if (navigation.errorText) throw new Error(navigation.errorText);
    // Navigation can replace the execution context; retry until the document is ready.
    for (;;) {
      if (cancelled) throw new Error('PDF generation cancelled');
      const state = await call('Runtime.evaluate', {
        expression: 'document.readyState === "complete" && !!window.Reveal && Reveal.isReady() && !!document.querySelector(".pdf-page")',
        returnByValue: true,
      }).catch(() => null);
      if (state?.result?.value) break;
      await new Promise(resolveDelay => setTimeout(resolveDelay, 100));
    }
    const ready = await call('Runtime.evaluate', {
      expression: `(async () => {
        await document.fonts.ready;
        if (window.MathJax?.startup?.promise) await MathJax.startup.promise;
        await new Promise(resolve => setTimeout(resolve, 250));
        return document.querySelectorAll('.pdf-page').length;
      })()`,
      awaitPromise: true, returnByValue: true,
    });
    if (ready.exceptionDetails || !ready.result.value) throw new Error('PDF layout initialization failed');
    const { data } = await call('Page.printToPDF', {
      printBackground: true, preferCSSPageSize: true, displayHeaderFooter: false,
      marginTop: 0, marginBottom: 0, marginLeft: 0, marginRight: 0,
    });
    const pdf = Buffer.from(data, 'base64');
    if (!pdf.subarray(0, 5).equals(Buffer.from('%PDF-')) || !pdf.subarray(-1024).includes(Buffer.from('%%EOF'))) {
      throw new Error('Chrome returned an incomplete PDF');
    }
    // Stage beside the destination so rename is atomic, including across volumes.
    const destination = resolve(output);
    const staged = `${destination}.${process.pid}.tmp`;
    try {
      await writeFile(staged, pdf);
      await rename(staged, destination);
    } finally {
      await rm(staged, { force: true });
    }
    console.log(`Created ${output} (${ready.result.value} slide pages)`);
  })()]);
} catch (error) {
  console.error(error.message);
  if (logs) console.error(logs);
  process.exitCode = 1;
} finally {
  cancelled = true;
  clearTimeout(timer);
  // This isolated Chrome can linger after printing on macOS; never touch other profiles.
  if (child.exitCode === null && child.signalCode === null) {
    const exited = new Promise(resolveExit => child.once('exit', resolveExit));
    child.kill('SIGTERM');
    const force = setTimeout(() => child.kill('SIGKILL'), 2000);
    await exited;
    clearTimeout(force);
  }
  await rm(profile, { recursive: true, force: true });
}
