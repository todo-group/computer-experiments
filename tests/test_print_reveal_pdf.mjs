// Integration tests: real Chrome/Reveal and Poppler, with no network resources.
// CHROME=/path/to/chrome REVEAL_DIST=/path/to/reveal/dist node --test tests/test_print_reveal_pdf.mjs
import test from 'node:test';
import assert from 'node:assert/strict';
import { execFile } from 'node:child_process';
import { promisify } from 'node:util';
import { mkdtemp, writeFile, readFile, rm } from 'node:fs/promises';
import { tmpdir } from 'node:os';
import { join, resolve } from 'node:path';
import { pathToFileURL, fileURLToPath } from 'node:url';

const exec = promisify(execFile);
const chrome = process.env.CHROME;
const dist = process.env.REVEAL_DIST;
const printer = fileURLToPath(new URL('../tools/print_reveal_pdf.mjs', import.meta.url));
const integration = { skip: !chrome || !dist, timeout: 30000 };

async function fixture(t, body, script = '') {
  const dir = await mkdtemp(join(tmpdir(), 'reveal-pdf-test-'));
  t.after(() => rm(dir, { recursive: true, force: true }));
  const html = join(dir, 'slides.html');
  const pdf = join(dir, 'slides.pdf');
  const url = name => pathToFileURL(resolve(dist, name)).href;
  await writeFile(html, `<!doctype html><html><head>
    <link rel="stylesheet" href="${url('reveal.css')}">
    <style>.reveal {font: 24px sans-serif} h2 {font-size:32px}
      .reveal ul {display:inline-block} .reveal li {line-height:48px}
    </style></head><body><div class="reveal"><div class="slides">${body}</div></div>
    <script src="${url('reveal.js')}"></script><script>${script}
    Reveal.initialize({width:800,height:500,margin:0.1,center:false,
      transition:'none',pdfSeparateFragments:false});</script></body></html>`);
  const print = (timeout = '15') => exec(process.execPath, [printer, chrome, html, pdf, timeout]);
  return { html, pdf, print };
}

test('long lists, delayed math and lazy images retain all content on one page per slide', integration, async t => {
  const svg = 'data:image/svg+xml,' + encodeURIComponent('<svg xmlns="http://www.w3.org/2000/svg" width="500" height="100"><text x="10" y="50" font-size="32">LAZY_IMAGE</text></svg>');
  const items = Array.from({length:12}, (_, i) => `<li>ITEM_${String(i + 1).padStart(2, '0')}</li>`).join('');
  const f = await fixture(t, `<section><h2>Long list</h2><ul>${items}</ul></section>
    <section><h2>Delayed math</h2><span class="math">pending</span></section>
    <section><h2>Lazy image</h2><img loading="lazy" data-src="${svg}"></section>`, `
    window.MathJax = {
      startup: {promise: new Promise(resolve => setTimeout(() => {
        document.querySelector('.math').innerHTML = '<mjx-container>READY_MATH</mjx-container>';
        resolve();
      }, 1200))},
      typesetPromise: async () => {}
    };`);
  await f.print();
  const { stdout } = await exec('pdftotext', ['-layout', f.pdf, '-']);
  const pages = stdout.trimEnd().split('\f');
  assert.equal(pages.length, 3);
  for (let i = 1; i <= 12; i++) assert.ok(pages[0].includes(`ITEM_${String(i).padStart(2, '0')}`));
  assert.ok(pages[1].includes('READY_MATH'));
  assert.ok(pages[2].includes('LAZY_IMAGE'));
  assert.ok(!stdout.includes('pending'));
});

test('a missing image fails without replacing an existing PDF', integration, async t => {
  const f = await fixture(t, '<section><h2>Missing asset</h2><img data-src="missing.svg"></section>');
  await writeFile(f.pdf, 'previous good PDF');
  await assert.rejects(f.print(), error => /Image failed to decode|Resource failed:.*missing\.svg/.test(error.stderr));
  assert.equal(await readFile(f.pdf, 'utf8'), 'previous good PDF');
});

test('unavailable MathJax fails instead of printing raw mathematics', integration, async t => {
  const f = await fixture(t, '<section><h2>Math</h2><span class="math">x^2</span></section>');
  await assert.rejects(f.print(), error => /MathJax 3\/4 did not load/.test(error.stderr));
});

test('a page script error fails instead of reporting a successful export', integration, async t => {
  const f = await fixture(t, '<section><h2>Broken script</h2></section>',
    'setTimeout(() => { throw new Error("SCRIPT_FAILURE"); }, 0);');
  await assert.rejects(f.print(), error => /SCRIPT_FAILURE/.test(error.stderr));
});

test('invalid timeouts fail before launching Chrome', async () => {
  await assert.rejects(exec(process.execPath, [printer, 'unused', 'input.html', 'output.pdf', 'NaN']),
    error => /positive finite number/.test(error.stderr));
});
