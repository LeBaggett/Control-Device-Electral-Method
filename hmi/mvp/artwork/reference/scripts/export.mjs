import { chromium } from 'playwright';
import { spawn } from 'node:child_process';
import fs from 'node:fs/promises';
import path from 'node:path';

const args = process.argv.slice(2);
const screen = args[0] ?? 'autotest';
const scaleIndex = args.indexOf('--scale');
const outIndex = args.indexOf('--out');
const scale = scaleIndex >= 0 ? Number(args[scaleIndex + 1]) : 2;
const out = outIndex >= 0 ? args[outIndex + 1] : `output/${screen}@${scale}x.png`;

if (!Number.isFinite(scale) || scale <= 0) {
  throw new Error('--scale deve ser um número positivo.');
}

const port = 4173;
const host = '127.0.0.1';
const url = `http://${host}:${port}/?screen=${encodeURIComponent(screen)}`;

const server = spawn(
  process.platform === 'win32' ? 'npx.cmd' : 'npx',
  ['vite', '--host', host, '--port', String(port)],
  { stdio: 'ignore' },
);

async function waitForServer(timeoutMs = 15000) {
  const start = Date.now();
  while (Date.now() - start < timeoutMs) {
    try {
      const r = await fetch(url);
      if (r.ok) return;
    } catch {}
    await new Promise((resolve) => setTimeout(resolve, 180));
  }
  throw new Error('Vite não iniciou dentro do tempo esperado.');
}

let browser;
try {
  await waitForServer();
  browser = await chromium.launch({ headless: true });
  const context = await browser.newContext({
    viewport: { width: 1600, height: 960 },
    deviceScaleFactor: scale,
  });
  const page = await context.newPage();
  await page.goto(url, { waitUntil: 'networkidle' });
  const root = page.locator('[data-screen-root]');
  await root.waitFor();
  const target = path.resolve(out);
  await fs.mkdir(path.dirname(target), { recursive: true });
  await root.screenshot({ path: target });
  console.log(target);
} finally {
  if (browser) await browser.close();
  server.kill('SIGTERM');
}
