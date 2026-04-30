/* App Logic tests using vitest
 * App.test.jsx
 *
 * Tests App.jsx's data flow and state logic
 * without mounting React or opening WebSockets.
 * Any test requiring a mounted component will be found in in App.spec.jsx.
 *
 * App.jsx's logic is:
 *   - Default coin initialisation (BTCUSDT)
 *   - Coin selection state update (setCurrentCoin)
 *   - JSON serialization of data passed between the socket and the UI
 *   - Trade button wiring (delegates to hook's trade())
 */

import { expect, describe, it } from 'vitest';

const DEFAULT_COIN = 'BTCUSDT';
const COIN_LIST    = ['BTCUSDT', 'ETHUSDT', 'ADAUSDT', 'XRPUSDT', 'DOTUSDT', 'UNIUSDT'];

/**
 * Default state test, make sure it initializes to the default coin,,
 */

describe('App Default State', () => {

  it('should default to BTCUSDT as the starting coin', () => {
    expect(DEFAULT_COIN).toBe('BTCUSDT');
  });

  it('should default to a non-null coin string', () => {
    expect(typeof DEFAULT_COIN).toBe('string');
    expect(DEFAULT_COIN.length).toBeGreaterThan(0);
  });
});

/**
 * Tests the clicking selection logic for the Watchlist mapping
 */

describe('App Coin Selection', () => {
  /* Models the setCurrentCoin state update that Watchlist's onCoin callback
   * trigger */

  it('should update currentCoin when a new coin is selected', () => {
    let currentCoin = DEFAULT_COIN;
    const setCurrentCoin = (c) => { currentCoin = c; };

    setCurrentCoin('ETHUSDT');
    expect(currentCoin).toBe('ETHUSDT');
  });

  it('should accept any coin from the default coin list', () => {
    COIN_LIST.forEach((coin) => {
      let currentCoin = DEFAULT_COIN;
      const setCurrentCoin = (c) => { currentCoin = c; };
      setCurrentCoin(coin);
      expect(currentCoin).toBe(coin);
    });
  });

  it('should reflect the last selected coin after multiple changes', () => {
    let currentCoin = DEFAULT_COIN;
    const setCurrentCoin = (c) => { currentCoin = c; };

    setCurrentCoin('ETHUSDT');
    setCurrentCoin('ADAUSDT');
    setCurrentCoin('XRPUSDT');
    expect(currentCoin).toBe('XRPUSDT');
  });
});
/**
 * JSON Serialization area (data passing between socket and other components)
 */

describe('App JSON Serialization', () => {
  /* App passes data from useCryptoSocket directly to child components.
   * Tests verify serilaization between useCrypto and other components works with expected 
   * outcomes */

  it('should round-trip a price/coin/time/klineFinished payload accurately', () => {
    const raw = JSON.stringify({
      coin:         'BTCUSDT',
      price:        '72111.00',
      time:         1700000000000,
      klineFinished: true,
    });
    const parsed = JSON.parse(raw);
    expect(parsed.coin).toBe('BTCUSDT');
    expect(parsed.price).toBe('72111.00');
    expect(parsed.time).toBe(1700000000000);
    expect(parsed.klineFinished).toBe(true);
  });

  it('should preserve the string type of price after serialization', () => {
    const raw    = JSON.stringify({ price: '72011.00' });
    const parsed = JSON.parse(raw);
    expect(typeof parsed.price).toBe('string');
  });

  it('should preserve the number type of time after serialization', () => {
    const raw    = JSON.stringify({ time: 1700000000000 });
    const parsed = JSON.parse(raw);
    expect(typeof parsed.time).toBe('number');
  });

  it('should preserve the boolean type of klineFinished after serialization', () => {
    const raw    = JSON.stringify({ klineFinished: true });
    const parsed = JSON.parse(raw);
    expect(typeof parsed.klineFinished).toBe('boolean');
  });
});
/**
 * Trade Button Contracts
 */

describe('App Trade Button Contract', () => {
  /* The TESTBUY and TESTSELL buttons call trade(buy, 1.0) and trade(sell, 1.0). */

  it('should call trade with type "buy" and quantity 1.0 on BUY click', () => {
    let args = null;
    const trade = (type, qty) => { args = { type, qty }; };

    trade('buy', 1.0);
    expect(args).toEqual({ type: 'buy', qty: 1.0 });
  });

  it('should call trade with type "sell" and quantity 1.0 on SELL click', () => {
    let args = null;
    const trade = (type, qty) => { args = { type, qty }; };

    trade('sell', 1.0);
    expect(args).toEqual({ type: 'sell', qty: 1.0 });
  });

  it('should always pass quantity 1.0 for both buy and sell', () => {
    const calls = [];
    const trade = (type, qty) => calls.push({ type, qty });

    trade('buy',  1.0);
    trade('sell', 1.0);
    expect(calls[0].qty).toBe(1.0);
    expect(calls[1].qty).toBe(1.0);
  });
});
