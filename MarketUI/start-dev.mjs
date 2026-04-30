/**
 * start-dev.mjs
 *
 * Dev startup script. Checks if the C++ backend is already running on port 8080.
 * If not, attempts to spawn Binance_Websockets.exe from the build/Debug directory.
 * Then starts the Vite dev server.
 *
 * Usage: npm start
 */

import net from 'net';
import { spawn } from 'child_process';
import { existsSync } from 'fs';
import { resolve, dirname } from 'path';
import { fileURLToPath } from 'url';

const __dirname = dirname(fileURLToPath(import.meta.url));

const BACKEND_PORT = 8080;
const BACKEND_EXE = resolve(__dirname, '../build/Debug/Binance_Websockets.exe');
const BACKEND_STARTUP_DELAY_MS = 2000;

function isPortListening(port) {
  return new Promise((res) => {
    const socket = net.createConnection({ port, host: '127.0.0.1' });
    socket.on('connect', () => { socket.destroy(); res(true); });
    socket.on('error', () => res(false));
  });
}

async function main() {
  const alreadyRunning = await isPortListening(BACKEND_PORT);

  if (alreadyRunning) {
    console.log('[start-dev] Backend already running on port 8080.');
  } else {
    if (!existsSync(BACKEND_EXE)) {
      console.error(`[start-dev] Backend executable not found at:\n  ${BACKEND_EXE}`);
      console.error('[start-dev] Build the C++ backend first — see docs/Build_Instructions.md');
      process.exit(1);
    }

    console.log('[start-dev] Starting backend...');
    const backend = spawn(BACKEND_EXE, [], {
      cwd: resolve(__dirname, '../build/Debug'),
      detached: false,
      stdio: 'inherit',
    });

    backend.on('error', (err) => {
      console.error('[start-dev] Failed to start backend:', err.message);
    });

    // Give the backend time to bind ports before Vite loads the page
    await new Promise((r) => setTimeout(r, BACKEND_STARTUP_DELAY_MS));
  }

  // Start Vite
  const isWindows = process.platform === 'win32';
  const vite = spawn(isWindows ? 'npx.cmd' : 'npx', ['vite'], {
    cwd: __dirname,
    stdio: 'inherit',
  });

  vite.on('close', (code) => process.exit(code ?? 0));
}

main();
