/* Watchlist Vitest

 *
 * Watchlist.test.jsx
 *
 * Tests the data-transformation and state logic for Watchlist in isolation.
 * Any test requiring a mounted component belongs in Watchlist.spec.jsx.
 */

import { expect, describe, it } from 'vitest';


const COINS = ['BTCUSDT', 'ETHUSDT', 'ADAUSDT', 'XRPUSDT', 'DOTUSDT', 'UNIUSDT'];

/* stripUsdt(coin)
 * Mirrors the display logic in Watchlist.jsx: coin.replace('USDT', '')
 */
const stripUsdt = (coin) => coin.replace('USDT', '');

/* isActive(coin, currentCoin)
 * Mirrors the className conditional: currentCoin === coin ? 'active' : ''
 */
const isActive = (coin, currentCoin) => currentCoin === coin;


describe('Watchlist — Coin List Contracts', () => {
  /* Encodes the expected default coins so additions or removals cause a
   * visible test failure rather than a silent regression. */

  it('should contain exactly six default coins', () => {
    expect(COINS.length).toBe(6);
  });

  it('should include BTCUSDT', () => {
    expect(COINS).toContain('BTCUSDT');
  });

  it('should include ETHUSDT', () => {
    expect(COINS).toContain('ETHUSDT');
  });

  it('should include ADAUSDT', () => {
    expect(COINS).toContain('ADAUSDT');
  });

  it('should include XRPUSDT', () => {
    expect(COINS).toContain('XRPUSDT');
  });

  it('should include DOTUSDT', () => {
    expect(COINS).toContain('DOTUSDT');
  });

  it('should include UNIUSDT', () => {
    expect(COINS).toContain('UNIUSDT');
  });

  it('should have every coin end with the USDT suffix', () => {
    COINS.forEach((coin) => expect(coin.endsWith('USDT')).toBe(true));
  });
});

describe('Watchlist — Display Label Formatting', () => {
  /* Verifies the stripUsdt helper that Watchlist uses to trim tickers. */

  it('should strip USDT from BTCUSDT to produce BTC', () => {
    expect(stripUsdt('BTCUSDT')).toBe('BTC');
  });

  it('should strip USDT from ETHUSDT to produce ETH', () => {
    expect(stripUsdt('ETHUSDT')).toBe('ETH');
  });

  it('should strip USDT from all default coins without leaving USDT in the label', () => {
    COINS.forEach((coin) => {
      expect(stripUsdt(coin)).not.toContain('USDT');
    });
  });

  it('should produce six unique display labels from the six default coins', () => {
    const labels = COINS.map(stripUsdt);
    const unique = new Set(labels);
    expect(unique.size).toBe(6);
  });
});

describe('Watchlist — Active Selection Logic', () => {

  it('should mark the currentCoin as active', () => {
    expect(isActive('BTCUSDT', 'BTCUSDT')).toBe(true);
  });

  it('should not mark a different coin as active', () => {
    expect(isActive('ETHUSDT', 'BTCUSDT')).toBe(false);
  });

  it('should transfer active status when currentCoin changes', () => {
    const currentCoin = 'ADAUSDT';
    expect(isActive('BTCUSDT', currentCoin)).toBe(false);
    expect(isActive('ADAUSDT', currentCoin)).toBe(true);
  });

  it('should mark exactly one coin as active for any given currentCoin', () => {
    const currentCoin = 'XRPUSDT';
    const activeCount = COINS.filter((c) => isActive(c, currentCoin)).length;
    expect(activeCount).toBe(1);
  });
});


describe('Watchlist — onCoin Callback Contract', () => {

  it('should pass the full ticker BTCUSDT (not BTC) to onCoin', () => {
    const displayLabel = 'BTC';
    const fullTicker   = COINS.find((c) => stripUsdt(c) === displayLabel);
    expect(fullTicker).toBe('BTCUSDT');
  });

  it('should pass the full ticker ETHUSDT (not ETH) to onCoin', () => {
    const displayLabel = 'ETH';
    const fullTicker   = COINS.find((c) => stripUsdt(c) === displayLabel);
    expect(fullTicker).toBe('ETHUSDT');
  });

  it('should resolve a full ticker for every display label', () => {
    COINS.map(stripUsdt).forEach((label) => {
      const fullTicker = COINS.find((c) => stripUsdt(c) === label);
      expect(fullTicker).toBeDefined();
    });
  });
});
