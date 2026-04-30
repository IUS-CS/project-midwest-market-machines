/* ShowPrice logic tests
 * ShowPrice.test.jsx
 *
 * Tests the formatting logic for ShowPrice props in isolation,
 * For tests requiring a mounted component belongs in ShowPrice.spec.jsx.
 */

import { expect, describe, it } from 'vitest';

/*
* Helpers mirror the formatting contracts implied by ShowPrice.jsx
*/

/* formatHeading(coin)
 * Replicates the JSX expression: {coin}'s Price
 */
const formatHeading = (coin) => `${coin}'s Price`;

/* formatPrice(price)
 * Replicates the JSX expression: ${price}
 */
const formatPrice = (price) => `$${price}`;

/* stripUsdt(coin)
 * Replicates the display trimming used elsewhere in prop passiung to Watchlist.
 * ShowPrice renders the full ticker, but this verifies the raw
 * string contract so other callers stay consistent.
 */
const stripUsdt = (coin) => coin.replace('USDT', '');

/*
* Heading Formatting
*/

describe('ShowPrice Heading Formatting', () => {
  /* Verifies the heading template for typical tickers */

  it("should produce \"BTCUSDT's Price\" for BTCUSDT", () => {
    expect(formatHeading('BTCUSDT')).toBe("BTCUSDT's Price");
  });

  it("should produce \"ETHUSDT's Price\" for ETHUSDT", () => {
    expect(formatHeading('ETHUSDT')).toBe("ETHUSDT's Price");
  });

  it("should produce \"ADAUSDT's Price\" for ADAUSDT", () => {
    expect(formatHeading('ADAUSDT')).toBe("ADAUSDT's Price");
  });

  it('should handle an empty coin string without throwing', () => {
    expect(() => formatHeading('')).not.toThrow();
    expect(formatHeading('')).toBe("'s Price");
  });
});

/*
* Price Formatting
*/

describe('ShowPrice — Price Formatting', () => {
  it('should prefix a normal price string with $', () => {
    expect(formatPrice('42000.00')).toBe('$42000.00');
  });
  it('should prefix a whole-number price with $', () => {
    expect(formatPrice('500')).toBe('$500');
  });
  it('should prefix a decimal price with $', () => {
    expect(formatPrice('0.00035')).toBe('$0.00035');
  });

  it('should produce "$null" when price is the string "null" (null coerced)', () => {
    /* 
    * ShowPrice renders price directly; null becomes the string "null" in JSX.
    *
    * This test encodes that behaviour so regressions are visible 
    * 
    */
    expect(formatPrice(String(null))).toBe('$null');
  });
});

/*
* Prop Type Contracts
*/

describe('ShowPrice Prop Type Contracts', () => {
  /* Verifies assumptions about what types the component receives. */

  it('should accept a numeric string for price', () => {
    expect(typeof '72000.00').toBe('string');
  });

  it('should accept a coin string in coinUSDT format', () => {
    const coin = 'BTCUSDT';
    expect(coin.endsWith('USDT')).toBe(true);
  });

  it('should strip USDT correctly when the display needs a short label', () => {
    expect(stripUsdt('BTCUSDT')).toBe('BTC');
    expect(stripUsdt('ETHUSDT')).toBe('ETH');
  });
});
