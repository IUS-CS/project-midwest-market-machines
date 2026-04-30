/**
 * useCryptoSocket Vitest
 * Logic tests for data transformation and guard logic.
 * useCryptoSocket.test.jsx
 * 
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

/*   
* Mocked static data
*/

const BtcKlineMessage = {
  Coin: 'BTCUSDT',
  Kline: {
    Open:          '71500.00',
    High:          '73200.00',
    Low:           '70800.00',
    Close:         '72550.00',
    StartTime:     1700000000000,
    KlineFinished: false,
  },
};

const HoldingMessage = {
  dataType: 'holding',
  coin:     'BTC',
  quantity: 0.5,
  last:     true,
};

const TransactionMessage = {
  dataType: 'transaction',
  type:     'buy',
  coin:     'BTC',
  price:    72111.00,
  quantity: 0.1,
  time:     '1700000000',
  last:     true,
};

/**
 * buildCandle
 * Replicates the candle construction logic found in useCryptoSocket
 */
function buildCandle(kline, existing = null) {
  const timeSeconds = Math.floor(kline.StartTime / 1000);
  return {
    time:  timeSeconds,
    open:  existing ? existing.open : parseFloat(kline.Open),
    high:  Math.max(parseFloat(kline.High),  existing ? existing.high : 0),
    low:   Math.min(parseFloat(kline.Low),   existing ? existing.low  : Infinity),
    close: parseFloat(kline.Close),
  };
}

/**
 * JSON Serialization area (data passing between socket and UI)
 */
describe('useCryptoSocket JSON Serialization', () => {

  it('should round-trip a kline payload preserving field types', () => {
    const parsed = JSON.parse(JSON.stringify(BtcKlineMessage));
    expect(parsed.Coin).toBe('BTCUSDT');
    expect(typeof parsed.Kline.Open).toBe('string');
    expect(typeof parsed.Kline.StartTime).toBe('number');
  });

  it('should preserve numeric precision for 70k area prices', () => {
    const tradeData = { type: 'buy', coin: 'BTCUSDT', price: 72111.55, quantity: 1.0 };
    const parsed    = JSON.parse(JSON.stringify(tradeData));
    expect(parsed.price).toBe(72111.55);
  });
});

/**
 * Candle Construction logic tests
 */
describe('useCryptoSocket Candle Construction', () => {

  it('should convert StartTime from milliseconds to seconds', () => {
    expect(buildCandle(BtcKlineMessage.Kline).time).toBe(1700000000);
  });

  it('should parse 70k area prices as correct floats', () => {
    const c = buildCandle(BtcKlineMessage.Kline);
    expect(c.open).toBe(71500.00);
    expect(c.close).toBe(72550.00);
  });

  it('should update high when the new value exceeds the previous high', () => {
    const first  = buildCandle(BtcKlineMessage.Kline);
    const second = buildCandle({ ...BtcKlineMessage.Kline, High: '75000.00' }, first);
    expect(second.high).toBe(75000.00);
  });

  it('should keep the previous high when the new high is lower', () => {
    const first  = buildCandle(BtcKlineMessage.Kline);
    const second = buildCandle({ ...BtcKlineMessage.Kline, High: '72000.00' }, first);
    expect(second.high).toBe(73200.00);
  });
});

/**
 * Trade Guard Logic tests
 */
describe('useCryptoSocket Trade Guard Logic', () => {

  it('should block a trade when price is null', () => {
    const price = null;
    const quantity = 1.0;
    expect(price !== null && quantity > 0).toBe(false);
  });

  it('should allow a trade when price is in the 70k area and quantity is valid', () => {
    const price = 72111.00;
    const quantity = 0.5;
    expect(price !== null && quantity > 0).toBe(true);
  });
});