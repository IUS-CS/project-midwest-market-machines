[![License](https://img.shields.io/badge/license-O'Saasy-darkblue.svg)](LICENSE)

# Simple Trade

Simple Trade is an AI-assisted paper trading application for six cryptocurrencies, as follows:

| Ticker | Full name |
| -------------- | --------------- |
| BTCUSDT | Bitcoin |
| ETHUSDT | Etheruem |
| ADAUSDT | Cardano |
| XRPUSDT | XRP |
| DOTUSDT | Polkadot |
| UNIUSDT | Uniswap |

On launching the application, you will see BTCUSDT selected as the default coin. From there, you may check the Watchlist
to select any of the six available coins. There are buttons provided to *BUY* and *SELL* coins.
A table is included beneath the chart that shows your current holdings.
The '?' icon pop-up in the bottom right hand corner is the chat box for interacting with the LLM.

The LLM will perform an ""

Historical data is pulled in via a `<coin><timeframe>.csv` file in `userData/histroical/`.
Transactions and current holdings are held in `userData/holdings.csv` and `userData/transactionHistory.csv` respectively.

This application runs entirely on-device, with no calls out to third parties other than Binance US' WebSocket API.

## Installation

### Dependencies

There are a few requirements to build and run Simple Trade:

1. [Node.js](https://nodejs.org/en/download)
2. [CMAKE](https://cmake.org/) v3.24 or greater
3. [Ollama](https://ollama.com/). We recommend using [qwen2.5:1.5b](https://ollama.com/library/qwen2.5:1.5b)

For Windows users, you will need nmake and its dependencies - notably Visual Studio.

To check if Node.js is installed, open your operating system's terminal and type `node -v` and `npm -v`. If the command isn't recognized, you can download it [here](https://nodejs.org/en/download).

Check for CMake version with `cmake --version`.

Check for Ollama with `ollama --version`. Then, `ollama list` to list models. You may use any model you choose.

### Building

#### NPM Install

1. Navigate to the `MarketUI` directory for the project.
2. If on Windows - ensure your ExecutionPolicy is set to allow scripts to be run from the terminal. You may use `Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass` to accomplish this.
3. Run the following commands for Windows and Unix systems respectively:

PowerShell

```PowerShell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass # Included from Step 2.
npm install --legacy-peer-deps                             # Install but ignore peer dependency conflicts.
```

Bash

```bash
npm install
npm run build                                 # Build the frontend to `dist/index.html`
```

#### Building and running

Then, in the project's root directory, run the following commands:

1. Make a `build/` directory.
2. `cd` into it.
3. Call `cmake` with `-G Ninja` to use Ninja for parallel builds.
4. Run `ninja` to compile the program from the build files.
5. Run the project from `./Binance_Websockets`

PowerShell

```PowerShell
mkdir build; cd build; cmake .. -G Ninja; ninja; .\Binance_Websockets
```

Bash

```bash
mkdir build/ && cd build/ && cmake .. -G Ninja && ninja && ./Binance_Websockets
```

`CmakeLists.txt` is configured to build the frontend into a single file, located at `MarketUI/dist/index.html`.
As such, there is no need to run `npm build`.

## Testing

The project's tests are separated into frontend and backend tests.

### Testing the frontend

There are two categories of frontend tests: vitest and Playwright.

To run all tests, use the commands below:

PowerShell

```PowerShell
cd MarketUI; npm run test; npx playwright test --workers=3 --config playwright-ct.config.cjs
```

Bash

```Bash
cd MarketUI/ && npm run test && npx playwright test --workers=3 --config playwright-ct.config.cjs
```

### Testing the backend

All backend tests are run through `ctest`.

PowerShell

```PowerShell
cd build; ctest
```

```Bash
cd build/ && ctest
```

It is not recommended to run tests in parallel, as they tend to fail more frequently.

For Windows, cmake uses vcpkg, which downloads the dependencies as the compiled binaries. We expect 49 tests which all pass.
On UNIX, we download the source for all dependencies direct, and expect 204 tests, of which 12 regularly fail:

```bash
94% tests passed, 12 tests failed out of 204

Total Test time (real) = 142.72 sec

The following tests FAILED:
  25 - zlib_find_package_configure (Failed)
  26 - zlib_find_package_build (Not Run)
  27 - zlib_find_package_test (Not Run)
  28 - zlib_find_package_no_components_configure (Failed)
  30 - zlib_add_subdirectory_configure (Failed)
  31 - zlib_add_subdirectory_build (Not Run)
  32 - zlib_add_subdirectory_test (Not Run)
  33 - zlib_add_subdirectory_exclude_configure (Failed)
  34 - zlib_add_subdirectory_exclude_build (Not Run)
  35 - zlib_add_subdirectory_exclude_test (Not Run)
 174 - IXSocketTest (Failed)
 188 - IXHttpServerTest (Failed)
Errors while running CTest
```

In either scenario, the project-specific tests are tests 1-17. You may run just these tests with this command:

```bash
ctest -I 1,17
```

## The Launch Page

![Launch Page](https://github.com/IUS-CS/project-midwest-market-machines/blob/main/docs/Simple%20Trade.png)
