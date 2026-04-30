import React, { useState } from 'react';
import CandlestickChart from './CandlestickChart';

const SampleCandle  = { time: 1700000000, open: 1.00, high: 1.10, low: 0.90,  close: 1.05 };
const UpdatedCandle = { time: 1700000060, open: 1.05, high: 1.15, low: 0.95,  close: 1.10 };

export const CoinReset = () => {
  const [coin, setCoin] = useState('BTCUSDT');
  return (
    <>
      <CandlestickChart coin={coin} latestCandle={null} historicalCandles={[]} />
      <button data-testid="switch" onClick={() => setCoin('ETHUSDT')}>switch</button>
    </>
  );
};

export const CandleUpdater = () => {
  const [candle, setCandle] = useState(null);
  return (
    <>
      <CandlestickChart coin="BTCUSDT" latestCandle={candle} historicalCandles={[]} />
      <button data-testid="set" onClick={() => setCandle(SampleCandle)}>set</button>
      <button data-testid="update" onClick={() => setCandle(UpdatedCandle)}>update</button>
    </>
  );
};

export const HistoricalLoader = () => {
  const [history, setHistory] = useState([]);
  return (
    <>
      <CandlestickChart coin="BTCUSDT" latestCandle={null} historicalCandles={history} />
      <button data-testid="load" onClick={() => setHistory([SampleCandle, UpdatedCandle])}>load</button>
    </>
  );
};