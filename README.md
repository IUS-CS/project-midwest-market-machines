[![License](https://img.shields.io/badge/license-O'Saasy-darkblue.svg)](LICENSE)

# Simple Trade

Simple Trade is an AI-assisted trading application for major cryptocurrencies and stocks. This application runs entirely on-device.

## Installation

### Dependencies

Simple Trade requires that you have [Node.js](https://nodejs.org/en/download) and [CMAKE](https://cmake.org/) v3.24 or greater installed. For windows development, you will need nmake and its dependencies - notably Visual Studio.

To check if Node.js is installed, open your operating system's terminal and type `node -v` and `npm -v`. If the command isn't recognized, you can download it [here](https://nodejs.org/en/download).

Check for CMake version with `cmake --version`.

### Building

1. Navigate to the `MarketUI` directory for the project.
2. If on Windows - ensure your ExecutionPolicy is set to allow scripts to be run from the terminal. You may use `Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass` to accomplish this.
3. Run the following commands:

PowerShell

```PowerShell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass # Included from Step 2.
npm install --legacy-peer-deps                             # Install but ignore peer dependency conflicts.
npm run build                                              # Build the frontend to `dist/index.html`
```

Bash

```bash
npm install
npm run build                                 # Build the frontend to `dist/index.html`
```

Then, in the project's root directory, run the following commands:

PowerShell

```PowerShell
# Set current dir as source, ./build/ as build dir, and MiniGW Makefiles as the makefiles generator.
cmake -S . -B build/ -G "MinGW Makefiles"
cd build/
nmake
./Binance_Websockets.exe
```

Bash

```bash
cmake -S . -B build/                          # Set ./build/ as the build diretory, and build to it.
cd build/
make                                          # Make Binance_Websockets.cpp and other files.
./Binance_Websockets.exe                      # Run the application.
```

> [!TIP]
> You may get an error after running `npm install --legacy-peer-deps`, but it can be safely ignored.

## Testing

For the frontend, navigate to `Midwest Market Machines/MarketUI/` and run `npm test`.
This will run the current [vitest](https://vitest.dev/) test suite.

For the backend, navigate to `Midwest Market Machines/build` and run `ctest`.
There are currently two project-specific test files, `SmokeTest`, and `ExchangeClientTest`.
Though, all test suites for all dependencies will be ran as well.

## The Launch Page

![Launch Page](https://github.com/IUS-CS/project-midwest-market-machines/blob/main/docs/Simple%20Trade.png)


