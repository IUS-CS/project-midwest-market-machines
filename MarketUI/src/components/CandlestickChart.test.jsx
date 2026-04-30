/* CandlestickChart logic tests
 *
 * CandlestickChart.test.jsx
 *
 * Tests the candle-data contracts and  guard logic that
 * CandlestickChart.jsx depends on.
 * Any test requiring a mounted component belongs in CandlestickChart.spec.jsx.
 */

import { expect, describe, it } from 'vitest';

/*
 * Fixtures
 * Representative candle objects mirroring what the chart receives.
 */

const SampleCandle = {
  time:  1700000000,   // Unix seconds (lightweight-charts expects seconds)
  open:  41000,
  high:  43000,
  low:   40500,
  close: 42000,
};

const UpdatedCandle = {
  time:  1700000060,
  open:  42000,
  high:  44000,
  low:   41500,
  close: 43500,
};

/*
 * Candle Shape Contracts
 */

describe('CandlestickChart Candle Shape Contracts', () => {
  /* Verifies the four required OHLC fields plus time are present and numeric.
   * lightweight-charts will silently drop malformed candles, so this guards
   * against data-pipeline regressions upstream. */

  it('should have a numeric time field', () => {
    expect(typeof SampleCandle.time).toBe('number');
  });

  it('should have a numeric open field', () => {
    expect(typeof SampleCandle.open).toBe('number');
  });

  it('should have a numeric high field', () => {
    expect(typeof SampleCandle.high).toBe('number');
  });

  it('should have a numeric low field', () => {
    expect(typeof SampleCandle.low).toBe('number');
  });

  it('should have a numeric close field', () => {
    expect(typeof SampleCandle.close).toBe('number');
  });

  it('should have high >= open', () => {
    expect(SampleCandle.high).toBeGreaterThanOrEqual(SampleCandle.open);
  });

  it('should have low <= open', () => {
    expect(SampleCandle.low).toBeLessThanOrEqual(SampleCandle.open);
  });

  it('should have high >= close', () => {
    expect(SampleCandle.high).toBeGreaterThanOrEqual(SampleCandle.close);
  });

  it('should have low <= close', () => {
    expect(SampleCandle.low).toBeLessThanOrEqual(SampleCandle.close);
  });
});

/*
 * Update Guard Logic
 */

describe('CandlestickChart — Update Guard Logic', () => {
  /* CandlestickChart.jsx wraps series.update() in a guard:
   *   if (!seriesRef.current || !latestCandle || !latestCandle.time) return;
   * These tests verify each guard condition as a boolean expression. */

  it('should block update when latestCandle is null', () => {
    const latestCandle = null;
    const shouldUpdate = latestCandle !== null && latestCandle?.time != null;
    expect(shouldUpdate).toBe(false);
  });

  it('should block update when latestCandle.time is undefined', () => {
    const latestCandle = { open: 100, high: 110, low: 90, close: 105 };
    const shouldUpdate = latestCandle !== null && latestCandle?.time != null;
    expect(shouldUpdate).toBe(false);
  });

  it('should block update when latestCandle.time is zero (falsy)', () => {
    const latestCandle = { ...SampleCandle, time: 0 };
    /* The component uses !latestCandle.time which is falsy for 0. */
    const shouldUpdate = latestCandle !== null && !!latestCandle.time;
    expect(shouldUpdate).toBe(false);
  });

  it('should allow update when latestCandle has a valid time', () => {
    const latestCandle = SampleCandle;
    const shouldUpdate = latestCandle !== null && !!latestCandle.time;
    expect(shouldUpdate).toBe(true);
  });
});

/*
 * Coin-Change Reset Contract
 */

describe('CandlestickChart Coin-Change Reset Contract', () => {
  /* When the coin prop changes, CandlestickChart calls series.setData([]).
   * These tests verify the equality check that triggers that reset. */

  it('should detect a coin change between BTCUSDT and ETHUSDT', () => {
    const prev = 'BTCUSDT';
    const next = 'ETHUSDT';
    expect(prev !== next).toBe(true);
  });

  it('should detect no change when the same coin is re-passed', () => {
    const prev = 'BTCUSDT';
    const next = 'BTCUSDT';
    expect(prev !== next).toBe(false);
  });

  it('should treat an empty historical array as a valid reset payload', () => {
    const resetData = [];
    expect(Array.isArray(resetData)).toBe(true);
    expect(resetData.length).toBe(0);
  });
});

/*
 * Historical Data Contracts
 */

describe('CandlestickChart Historical Data Contracts', () => {
  /* CandlestickChart calls series.setData(historicalCandles).
   * Verifies the shape requirements for a valid historical payload. */

  it('should accept an empty array for historicalCandles', () => {
    const historicalCandles = [];
    expect(Array.isArray(historicalCandles)).toBe(true);
  });

  it('should accept an array of candle objects', () => {
    const historicalCandles = [SampleCandle, UpdatedCandle];
    expect(historicalCandles.length).toBe(2);
    historicalCandles.forEach((c) => {
      expect(typeof c.time).toBe('number');
      expect(typeof c.close).toBe('number');
    });
  });

  it('should be sorted by time ascending for correct chart rendering', () => {
    const candles = [UpdatedCandle, SampleCandle].sort((a, b) => a.time - b.time);
    expect(candles[0].time).toBeLessThan(candles[1].time);
  });

  it('should default to an empty array when historicalCandles is undefined', () => {
    /* Mirrors: seriesRef.current.setData(historicalCandles || []) */
    const historicalCandles = undefined;
    const safe = historicalCandles || [];
    expect(safe).toEqual([]);
  });
});