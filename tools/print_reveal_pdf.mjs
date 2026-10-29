#!/usr/bin/env node
// Typeset before paginating: waiting after ?print-pdf is already too late.
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
const timeoutMs = Number(timeoutSeconds) * 1000;
if (!Number.isFinite(timeoutMs) || timeoutMs <= 0) {
  console.error('TIMEOUT_SECONDS must be a positive finite number');
  process.exit(1);
}

// Runs in Chrome, not Node. Do not capture variables from the outer scope.
async function prepareSlides() {
  const root = Reveal.getRevealElement();
  // Reveal normally loads images near the current slide only. Load every slide
  // before measuring, including native lazy images and Reveal's data-src images.
  for (const img of root.querySelectorAll('img')) {
    img.loading = 'eager';
    for (const attr of ['src', 'srcset']) {
      if (img.hasAttribute(`data-${attr}`)) {
        img.setAttribute(attr, img.getAttribute(`data-${attr}`));
        img.removeAttribute(`data-${attr}`);
      }
    }
  }
  const waitForImages = async () => {
    await Promise.all([...root.querySelectorAll('img')].map(async img => {
      if (!img.hasAttribute('src') && !img.hasAttribute('srcset')) return;
      try { await img.decode(); }
      catch { throw new Error(`Image failed to decode: ${img.currentSrc || img.src}`); }
    }));
  };
  const typeset = async () => {
    if (!root.querySelector('.math, math, mjx-container')) return;
    if (!window.MathJax?.startup?.promise) {
      throw new Error('MathJax 3/4 did not load; check the script URL and network');
    }
    await MathJax.startup.promise;
    if (typeof MathJax.typesetPromise !== 'function') {
      throw new Error('MathJax initialization did not provide typesetPromise');
    }
    await MathJax.typesetPromise([root]);
    if (root.querySelector('[data-mjx-error], mjx-merror')) {
      throw new Error('MathJax reported an equation error');
    }
    if ([...root.querySelectorAll('.math')].some(el =>
      !el.querySelector('mjx-container') && el.textContent.trim())) {
      throw new Error('Unrendered mathematics remains on a slide');
    }
  };
  await waitForImages();
  await typeset();
  // MathJax can request additional fonts during typesetting. Wait afterwards.
  await document.fonts.ready;
  Reveal.layout();

  // Reveal 5 selects print view only during initialization; configure({view})
  // alone does not activate it. Recreate the deck using its public API, retaining
  // the already typeset DOM. Remove the query override before initializing.
  const RevealClass = window.Reveal;
  const config = { ...Reveal.getConfig(), view: 'print', pdfSeparateFragments: false };
  Reveal.destroy();
  history.replaceState(null, '', location.pathname);
  window.Reveal = new RevealClass(root, config);
  // Listen before initialization: pdf-ready is distinct from Reveal's ready.
  const pdfReady = new Promise(resolve => Reveal.on('pdf-ready', resolve));
  await Reveal.initialize();
  await pdfReady;
  await waitForImages();
  await document.fonts.ready;
  await new Promise(requestAnimationFrame);
  await new Promise(requestAnimationFrame);

  const pages = [...root.querySelectorAll('.pdf-page')];
  if (!pages.length) throw new Error('No PDF pages were created');
  const pageWidth = parseFloat(document.body.style.width);
  const pageHeight = parseFloat(document.body.style.height);
  const { width: slideWidth, height: slideHeight } = Reveal.getComputedSlideSize(innerWidth, innerHeight);
  const scaled = [];
  // Chromium treats inline-block lists as indivisible. If a list exceeds the
  // remaining page height it moves the entire body to another page, where the
  // Reveal wrapper can clip it. Fit the *whole* slide, including wide equations,
  // before printing. CSS zoom participates in pagination (transform does not).
  for (const [index, page] of pages.entries()) {
    const slide = page.querySelector('section');
    slide.style.setProperty('height', 'auto', 'important');
    slide.style.setProperty('min-height', '0', 'important');
    slide.style.left = '0px';
    slide.style.top = '0px';
    const origin = slide.getBoundingClientRect();
    const rects = [...slide.querySelectorAll('*')]
      .filter(el => !el.closest('mjx-assistive-mml, mjx-speech') &&
        getComputedStyle(el).visibility !== 'hidden')
      .map(el => el.getBoundingClientRect()).filter(rect => rect.width && rect.height);
    const left = Math.min(0, ...rects.map(rect => rect.left - origin.left));
    const top = Math.min(0, ...rects.map(rect => rect.top - origin.top));
    const width = Math.max(slideWidth, ...rects.map(rect => rect.right - origin.left)) - left;
    const height = Math.max(1, ...rects.map(rect => rect.bottom - origin.top)) - top;
    const scale = Math.min(1, slideWidth / width, slideHeight / height);
    slide.style.zoom = String(scale);
    slide.style.left = `${(pageWidth - width * scale) / (2 * scale) - left}px`;
    const marginTop = config.center || slide.classList.contains('center')
      ? (pageHeight - height * scale) / 2 : (pageHeight - slideHeight) / 2;
    slide.style.top = `${marginTop / scale - top}px`;
    // Leave 1 CSS pixel for Chromium's rounding at the page boundary.
    page.style.height = `${pageHeight - 1}px`;
    if (scale < 0.999) scaled.push({ slide: index + 1, scale });
  }
  await new Promise(requestAnimationFrame);
  await new Promise(requestAnimationFrame);
  for (const [index, page] of pages.entries()) {
    const slide = page.querySelector('section');
    const style = slide && getComputedStyle(slide);
    if (!slide || !slide.getBoundingClientRect().height ||
        style.display === 'none' || style.visibility === 'hidden' || style.opacity === '0') {
      throw new Error(`Slide ${index + 1} is hidden in print view`);
    }
  }
  return { slides: pages.length, scaled };
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
const pageErrors = [];
const resourceUrls = new Map();
const requiredResource = type => ['Script', 'Stylesheet', 'Image', 'Font'].includes(type);
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
    if (message.method === 'Runtime.exceptionThrown') {
      const details = message.params.exceptionDetails;
      pageErrors.push(details.exception?.description || details.text);
    }
    if (message.method === 'Network.requestWillBeSent') {
      resourceUrls.set(message.params.requestId, message.params.request.url);
    }
    if (message.method === 'Network.loadingFailed' && requiredResource(message.params.type)) {
      pageErrors.push(`Resource failed: ${resourceUrls.get(message.params.requestId)} (${message.params.errorText})`);
    }
    if (message.method === 'Network.responseReceived' && requiredResource(message.params.type) && message.params.response.status >= 400) {
      pageErrors.push(`HTTP ${message.params.response.status}: ${message.params.response.url}`);
    }
    const item = pending.get(message.id);
    if (!item) continue;
    pending.delete(message.id);
    if (message.error) item.reject(new Error(message.error.message));
    else item.resolve(message.result);
  }
});

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
    await call('Runtime.enable');
    await call('Network.enable');
    // Use the same CSS when measuring and printing.
    await call('Emulation.setEmulatedMedia', { media: 'print' });
    const url = pathToFileURL(resolve(input));
    url.search = 'view=slide';
    const navigation = await call('Page.navigate', { url: url.href });
    if (navigation.errorText) throw new Error(navigation.errorText);
    // Navigation can replace the execution context; retry until the document is ready.
    for (;;) {
      if (cancelled) throw new Error('PDF generation cancelled');
      const state = await call('Runtime.evaluate', {
        expression: 'document.readyState === "complete" && !!window.Reveal && Reveal.isReady()',
        returnByValue: true,
      }).catch(() => null);
      if (pageErrors.length) throw new Error(pageErrors.join('\n'));
      if (state?.result?.value) break;
      await new Promise(resolveDelay => setTimeout(resolveDelay, 100));
    }
    const ready = await call('Runtime.evaluate', {
      expression: `(${prepareSlides.toString()})()`,
      awaitPromise: true, returnByValue: true,
    });
    if (ready.exceptionDetails) {
      throw new Error(ready.exceptionDetails.exception?.description || ready.exceptionDetails.text);
    }
    if (pageErrors.length) throw new Error(pageErrors.join('\n'));
    if (!ready.result.value?.slides) throw new Error('PDF layout initialization failed');
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
    const { slides, scaled } = ready.result.value;
    console.log(`Created ${output} (${slides} slide pages)`);
    if (scaled.length) console.log('Fitted slides: ' + scaled.map(({ slide, scale }) =>
      `${slide} (${Math.round(scale * 100)}%)`).join(', '));
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
