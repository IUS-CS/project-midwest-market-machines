import { useState } from 'react';
import ShowPrice    from './components/ShowPrice';
import Watchlist    from './components/Watchlist';
/**
 * Stubs mock dependencies allowing for Playwright Component test App.spec.jsx,
 */

const StubCandlestickChart = ({ coin }) => (
  <div data-testid="candlestick-stub">{coin}</div>
);

const StubChatBox = () => <div data-testid="chatbox-stub" />;

const StubPurchasesTable = ({ data }) => (
  <div data-testid="purchases-stub">{data.length} rows</div>
);

const defaultHookState = {
  price:            '77777.00',
  latestCandle:     null,
  holdings:         [],
  historicalCandles:[],
  trade:            () => {},
};

/**
 * AppMockup
 * Replicates App.jsx's layout and state for testing.
 * props:
 * initialCoin - starting value of currentCoin
 * hookState   - overrides for mock data (price, holdings, trade fn)
 */
export const AppMockup = ({ initialCoin = 'BTCUSDT', hookState = {} }) => {
  const [currentCoin, setCurrentCoin] = useState(initialCoin);
  const { price, holdings, trade } = { ...defaultHookState, ...hookState };

  return (
    <>
      <div className="App_header">
        <h1>Simple Trade</h1>
        <p>Your one-stop shop for all things crypto!</p>
      </div>
      <div>
        <button onClick={() => trade('buy',  1.0)}>TEST BUY</button>
        <button onClick={() => trade('sell', 1.0)}>TEST SELL</button>
      </div>
      <div className="main_layout">
        <Watchlist onCoin={setCurrentCoin} currentCoin={currentCoin} />
        <div className="flex_row">
          <ShowPrice price={price} coin={currentCoin} />
          <StubCandlestickChart coin={currentCoin} />
        </div>
      </div>
      <StubPurchasesTable data={holdings} />
      <StubChatBox />
    </>
  );
};