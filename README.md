[![License](https://img.shields.io/badge/license-O'Saasy-darkblue.svg)](LICENSE)

# Simple Trade

Simple Trade is an AI-assisted trading application for major cryptocurrencies and stocks. This application runs entirely on-device.

## Installation

### Dependencies

Simple Trade requires that you have [Node.js](https://nodejs.org/en/download) and [CMAKE](https://cmake.org/) v3.5 or greater installed.

To check if Node.js is installed, open your operating system's terminal and type `node -v` and `npm -v`. If the command isn't recognized, you can download it [here](https://nodejs.org/en/download).

Check for CMake version with `cmake --version`.

### Building

In the project's root directory, run the following commands:

```bash
cmake -S . -B build/          # Set ./build/ as the build diretory, and build to it.
cd build/
./Binance_Websockets          # Run the backend.
```

Then,

1. Navigate to the `MarketUI` directory for the project. This can be done by copying the path when in the `MarketUI` folder, and then typing `cd [CopiedPath]\MarketUI` into your terminal (i.e. Command Prompt).
2. Run the following commands:

```bash
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
npm install --legacy-peer-deps
npm run dev
```

> [!TIP]
> You may get an error after running `npm install --legacy-peer-deps`, but it can be safely ignored.

After you run these commands, copy the `https://localhost:[numbers]/` address and paste it into your browser's address bar to get to the launch page.
Alternatively, you may press `b` from the vite dev terminal window to have vite launch a browser window for you.

## Testing

For the frontend, navigate to `Midwest Market Machines/MarketUI/` and run `npm test`.
This will run the current [vitest](https://vitest.dev/) test suite.

For the backend, navigate to `Midwest Market Machines/build` and run `ctest`.
This will run `SmokeTest`, a file you made while building with CMake.

## The Launch Page

![Launch Page](https://github.com/IUS-CS/project-midwest-market-machines/blob/main/docs/Simple%20Trade.png)

