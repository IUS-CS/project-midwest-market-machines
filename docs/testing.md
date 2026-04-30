## Testing

Simple Trade uses three testing tools that cover different layers of the application:

| Tool | Scope | Command |
|------|-------|---------|
| Vitest | Unit tests for components and hooks | `npm test` |
| Playwright Component Testing | Component-level integration tests in a real browser | `npx playwright test --workers=3 --config playwright-ct.config.cjs` |
| Playwright E2E | Full end-to-end tests against the running app | `npx playwright test` |

---

### Running the tests

**Unit tests (Vitest):**

```bash
cd MarketUI
npm test
```

Vitest runs in watch mode by default. Press `q` to quit, or run `npm test -- --run` for a single pass.

---

**Component tests (Playwright CT):**

```bash
cd MarketUI
npx playwright test --workers=3 --config playwright-ct.config.cjs
```

Playwright mounts individual components in isolation inside a real Chromium browser. The `--workers=3` flag runs three test workers in parallel, which is a reasonable default for local development.

---

**End-to-end tests (Playwright E2E):**

First, ensure the app is running on `localhost:5173` (the Vite dev server):

```bash
cd MarketUI
npm run dev
```

Then in a second terminal:

```bash
cd MarketUI
npx playwright test
```

---

### Generating new E2E tests with Codegen

Playwright's codegen tool records user interactions and generates test code automatically. This is a convenient way to write code for Playwright components, and enables automated test macros that behave like real users exploring the webpage:

```bash
npx playwright codegen localhost:5173
```

A browser window and a Playwright Inspector window open side by side. Interact with the app in the browser, when clicking, typing, and navigating the inspector records each action as test code. Copy the generated code into a new `.spec.js` file in the `tests/` directory.

---

### Test structure

```
MarketUI/
├── src/
│   └── components/
│       ├── Watchlist.test.jsx        # Vitest unit - coin selection, active state
│       ├── ShowPrice.test.jsx         # Vitest unit - price and coin label rendering
│       ├── CandlestickChart.test.jsx   # Vitest unit - chart mount, coin switching
│       ├── ChatBox.test.jsx            # Vitest unit - open/close, send message
│       └── *.spec.jsx                 # Playwright Component tests
└── playwright-ct.config.cjs          # Playwright CT configuration
├── tests/
│   └── *.spec.js                     # Playwright E2E tests
└── playwright-ct.config.cjs           # Playwright CT configuration
```

---

### What is tested

| Component / Module | Test type | What is verified |
|--------------------|-----------|-----------------|
| `Watchlist` | Vitest unit | Renders all six coins; fires `onCoin` on click; applies `.active` class to the selected coin. |
| `ShowPrice` | Vitest unit | Renders the coin label and price; handles `"Loading..."` string gracefully. |
| `CandlestickChart` | Vitest unit | Mounts without error; clears data when `coin` prop changes. |
| `ChatBox` | Vitest unit | Toggle opens and closes the window; Send button fires; Enter key fires. |
| `useCryptoSocket` | Playwright CT | WebSocket message handling; candle map accumulation; coin switch resets state; `trade()` blocked when price is null. |
| Full app flow | Playwright E2E | App loads; coin switching updates price and chart; Buy/Sell recorded in table. |

---

### Why some tests cannot be automated

The following behaviors require manual verification and cannot be fully automated:

**1. Live Binance data stream**  
The WebSocket connection to `ws://127.0.0.1:8080` depends on the C++ backend being running and Binance's API being reachable. Unit and component tests mock the WebSocket. Verifying that real market data flows end-to-end requires a manual smoke test with the full backend running.

**Manual test plan:**  
Launch `Binance_Websockets.exe`, open the app, confirm the price display updates at least once per minute, and confirm new candles appear on the chart.

**2. CSV persistence across sessions**  
Automated tests clear state between runs. Verifying that `userData/holdings.csv` and `userData/transactionHistory.csv` correctly persist across app restarts requires a manual check.

**Manual test plan:**  
Execute a buy trade, close the app, relaunch, and confirm the holding appears in the portfolio table, enabled by Playwright End to End

**3. LLM chat responses**  
The results of a local Qwen model response quality cannot be asserted deterministically. Automated tests can only verify that the chat sends a message and receives a non-empty response without error.