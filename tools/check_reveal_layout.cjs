#!/usr/bin/env node
// Read-only visual checks. Requires Playwright; use NODE_PATH for a separate install.
// Usage: node check_reveal_layout.cjs CHROME INPUT.html OUTPUT_DIRECTORY
const { chromium } = require('playwright');
const fs = require('node:fs/promises');
const path = require('node:path');
const { pathToFileURL } = require('node:url');

(async () => {
  const [executablePath, input, output] = process.argv.slice(2);
  if (!executablePath || !input || !output) {
    throw new Error('Usage: check_reveal_layout.cjs CHROME INPUT.html OUTPUT_DIRECTORY');
  }
  const browser = await chromium.launch({
    executablePath, headless: true,
    ...(process.env.REVEAL_TEST_PROXY ? { proxy: { server: process.env.REVEAL_TEST_PROXY } } : {}),
  });
  try {
    const page = await browser.newPage({
      viewport: { width: 1260, height: 840 },
      // Optional only for isolated CI environments with a TLS-intercepting proxy.
      ignoreHTTPSErrors: process.env.REVEAL_TEST_IGNORE_HTTPS_ERRORS === '1',
    });
    const errors = [];
    page.on('pageerror', e => errors.push(e.message));
    page.on('requestfailed', r => errors.push(`${r.url()}: ${r.failure()?.errorText}`));
    await page.goto(pathToFileURL(path.resolve(input)).href, { waitUntil: 'domcontentloaded', timeout: 60000 });
    await page.waitForFunction(() => window.Reveal?.isReady());
    await page.waitForFunction(() => !!window.MathJax?.startup?.promise, null, { timeout: 60000 });
    await page.evaluate(async () => {
      await document.fonts.ready;
      if (window.MathJax?.startup?.promise) await MathJax.startup.promise;
      Reveal.configure({ transition: 'none' });
      Reveal.layout();
    });
    const initial = await page.evaluate(() => ({
      mathjax: !!window.MathJax?.startup?.document,
      math: document.querySelectorAll('mjx-container').length,
      mathErrors: [...document.querySelectorAll('mjx-merror,[data-mjx-error]')].map(e => e.textContent),
      fonts: document.fonts.check('26px "BIZ UDPGothic"'),
      slides: Reveal.getSlides().length,
    }));
    const indices = await page.evaluate(() => Reveal.getSlides().map(s => Reveal.getIndices(s)));
    const report = [];
    await fs.mkdir(output, { recursive: true });
    for (let i = 0; i < indices.length; i++) {
      await page.evaluate(idx => Reveal.slide(idx.h, idx.v), indices[i]);
      await page.evaluate(() => new Promise(resolve => requestAnimationFrame(() => requestAnimationFrame(resolve))));
      const result = await page.evaluate(() => {
        const s = Reveal.getCurrentSlide();
        const box = document.querySelector('.slides').getBoundingClientRect();
        const elements = [...s.querySelectorAll('h1,h2,p,li,table,img,mjx-container')]
          .filter(e => !e.closest('.notes') && e.getBoundingClientRect().width > 0);
        const overflow = elements.map(e => ({
          tag: e.tagName, text: e.textContent?.slice(0, 100), rect: e.getBoundingClientRect().toJSON(),
        })).filter(e => e.rect.bottom > box.bottom + 2 || e.rect.top < box.top - 2 ||
          e.rect.right > box.right + 2 || e.rect.left < box.left - 2);
        return {
          title: s.querySelector('h1,h2')?.textContent, overflow,
          brokenImages: [...s.querySelectorAll('img')].filter(e => !e.complete || !e.naturalWidth).map(e => e.src),
        };
      });
      report.push({ page: i + 1, ...result });
      await page.screenshot({ path: path.join(output, `${String(i + 1).padStart(2, '0')}.png`) });
    }
    const problemSlides = report.filter(x => x.overflow.length || x.brokenImages.length);
    await fs.writeFile(path.join(output, 'report.json'), JSON.stringify({ initial, errors, slides: report }, null, 2));
    console.log(JSON.stringify({ initial, errors, problemSlides }, null, 2));
    if (!initial.mathjax || !initial.math || !initial.fonts || initial.mathErrors.length ||
        errors.length || problemSlides.length) process.exitCode = 1;
  } finally {
    await browser.close();
  }
})().catch(error => { console.error(error.message); process.exitCode = 1; });
