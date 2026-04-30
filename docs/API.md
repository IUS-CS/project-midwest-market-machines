## API Reference

> This section documents the public interface of each frontend component and the data hook. "Public interface" means the props a component accepts and the values a hook returns, and what you need to know to use or test each piece.
>
> The C++ backend's internal classes are intentionally omitted here; they are implementation details not visible to application developers. Their interfaces are documented via inline comments in the source files.

---

### `useCryptoSocket(selectedCoin)`

**Location:** `src/useCryptoSocket.jsx`  
**Type:** React custom hook  
**Purpose:** Manages both WebSocket connections to the backend. Builds and maintains the live candle stream, historical data, and paper trading state.

#### Parameters

| Name | Type | Description |
|------|------|-------------|
| `selectedCoin` | `string` | Binance trading pair symbol, e.g. `"BTCUSDT"`. Changing this clears the current candle buffer and begins streaming the new coin. |

#### Return value

| Name | Type | Description |
|------|------|-------------|
| `price` | `number \| string` | Latest close price for the selected coin. `"Loading..."` before data arrives. |
| `latestCandle` | `Candle \| null` | The most recently updated candle object for the selected coin. |
| `historicalCandles` | `Candle[]` | Merged array of shipped historical data and the live candle buffer, sorted by time. Used to seed the chart on load or coin switch. |
| `holdings` | `Holding[]` | Current paper trading holdings loaded from the backend CSV. |
| `transactions` | `Transaction[]` | Full transaction history loaded from the backend CSV. |
| `trade` | `(type: string, quantity: number) => void` | Sends a buy or sell trade to the backend over port 8081. Blocked if `price` is null or `quantity` is invalid. |

#### Candle shape

```ts
{
  time: number,   // Unix timestamp in seconds
  open: number,
  high: number,
  low: number,
  close: number
}
```

#### Holding shape

```ts
{
  dataType: "holding",
  coin: string,
  price: number,
  quantity: number
}
```

#### WebSocket URLs

| URL | Purpose |
|-----|---------|
| `ws://127.0.0.1:8080` | Live Kline stream |
| `ws://127.0.0.1:8081` | Database: historical data, holdings, trades |

#### Notes

- Switching `selectedCoin` resets `candleMapRef`, `lastCloseRef`, `latestCandle`, and `price` to avoid stale candle data appearing on the new chart.
- All coins are buffered in `allCoinsBuffer` even when not displayed, so switching back to a coin preserves its live candle history for the current session.

---

### `<App />`

**Location:** `src/App.jsx`  
**Type:** Root component  
**Purpose:** State coordinator. Owns `currentCoin`, calls `useCryptoSocket`, and renders all other components.

No external props as this is the application root.

---

### `<Watchlist onCoin currentCoin />`

**Location:** `src/components/Watchlist.jsx`  
**Purpose:** Displays the list of supported coins and fires a callback when the user selects one.

#### Props

| Prop | Type | Required | Description |
|------|------|----------|-------------|
| `onCoin` | `(coin: string) => void` | Yes | Callback fired with the selected coin symbol when the user clicks a list item. |
| `currentCoin` | `string` | Yes | The currently active coin symbol. Used to apply the `.active` CSS class to the selected item. |

#### Supported coins (hardcoded)

`BTCUSDT`, `ETHUSDT`, `ADAUSDT`, `XRPUSDT`, `DOTUSDT`, `UNIUSDT`

---

### `<ShowPrice price coin />`

**Location:** `src/components/ShowPrice.jsx`  
**Purpose:** Displays the name and current price of the selected coin.

#### Props

| Prop | Type | Required | Description |
|------|------|----------|-------------|
| `price` | `number \| string` | Yes | The current price to display. Renders as-is (e.g. `"Loading..."` is valid). |
| `coin` | `string` | Yes | The coin symbol used as the display label (e.g. `"BTCUSDT"`). |

---

### `<CandlestickChart coin latestCandle historicalCandles />`

**Location:** `src/components/CandlestickChart.jsx`  
**Purpose:** Manages the full lifecycle of a `lightweight-charts` candlestick series. Loads historical candles on mount or coin switch, then applies live updates as new candles arrive.

#### Props

| Prop | Type | Required | Description |
|------|------|----------|-------------|
| `coin` | `string` | Yes | The active coin symbol. Changing this clears the chart and loads new historical data. |
| `latestCandle` | `Candle \| null` | Yes | The latest live candle. The chart calls `series.update()` each time this value changes. |
| `historicalCandles` | `Candle[]` | Yes | Full sorted candle array used to seed the chart via `series.setData()`. |

#### Behavior notes

- The chart is created once on mount and destroyed on unmount. It is **not** recreated when `coin` changes, only the data is replaced.
- A `resize` listener keeps chart width in sync with its container.
- Live update errors (e.g. out-of-order timestamps) are caught and logged as warnings rather than thrown.

---

### `<ChatBox />`

**Location:** `src/components/ChatBox.jsx`  
**Purpose:** Floating AI chat interface. Renders a toggle button and a chat window. LLM integration is stubbed in the current release. Responses are placeholder strings pending Sprint 5 API integration.

#### Props

None. `ChatBox` manages its own state internally.

#### Internal state

| State | Type | Description |
|-------|------|-------------|
| `isOpen` | `boolean` | Controls chat window visibility. |
| `messages` | `Message[]` | Conversation history rendered in the chat window. |
| `input` | `string` | Controlled textarea value. |
| `isTyping` | `boolean` | Shows the typing indicator while waiting for an LLM response. |

#### `getLLMResponse(userMessage)` (stub)

Currently returns a random string from a hardcoded array after a 1-second artificial delay.

---

### `<Navbar />`

**Location:** `src/components/navBar.jsx`  
**Purpose:** Top navigation bar. Displays the "MARKET MACHINE" logo. Navigation links are planned for a future sprint.

#### Props

None.

---

### `<PurchasesTable columns data />`

**Location:** `src/components/purchases/PurchasesTable.jsx`  
**Purpose:** Renders the user's current holdings using a `shadcn/ui` DataTable.

#### Props

| Prop | Type | Required | Description |
|------|------|----------|-------------|
| `columns` | `ColumnDef[]` | Yes | Column definitions imported from `columns.jsx`. |
| `data` | `Holding[]` | Yes | Array of holding objects from `useCryptoSocket`. |
