import useWebSocket from 'react-use-websocket';
import { useState, useEffect, useRef } from 'react';

/* 
Backend websocket connection.
*/
const useCryptoSocket = (selectedCoin) => {
  const [price, setPrice] = useState(null);
  const SOCKET_URL = "ws://127.0.0.1:8080";

  const [latestCandle, setLatestCandle] = useState(null)
  const candleMapRef = useRef(new Map());
  const lastCloseRef = useRef(null)
  const prevCoinRef = useRef(null);

  const { lastJsonMessage } = useWebSocket(
    SOCKET_URL, {
    share: true,
    shouldReconnect: () => true,
    reconnectAttempts: 1000,
    reconnectInterval: 3000,
    onOpen: () => console.log('WebSocket opened'),
    onClose: () => console.log('WebSocket closed'),
  });

  useEffect(() => {
    if (prevCoinRef.current !== selectedCoin) {
      candleMapRef.current = new Map();
      lastCloseRef.current = null;
      setLatestCandle(null);
      setPrice(null);
      prevCoinRef.current = selectedCoin;
    }
  }, [selectedCoin])

  useEffect(() => {
    if ( !lastJsonMessage || lastJsonMessage.Coin !== selectedCoin || !lastJsonMessage.Kline) 
      { return; }

    const k = lastJsonMessage.Kline;
    const open = parseFloat(k.Open);
    const high = parseFloat(k.High);
    const low = parseFloat(k.Low);
    const close = parseFloat(k.Close);

    const timeSeconds = Math.floor(k.StartTime / 1000);
    const existingCandle = candleMapRef.current.get(timeSeconds)
    const realOpen = existingCandle ? existingCandle.open : (lastCloseRef.current ?? open);

    const candle = {
      time: timeSeconds,
      open: realOpen,
      high: Math.max(high, realOpen, close),
      low: Math.min(low, realOpen, close),
      close: close,
    };

    candleMapRef.current.set(timeSeconds, candle);

    if (k.KlineFinished) { lastCloseRef.current = close; }

    const sorted = Array.from(candleMapRef.current.values()).sort((a, b) => a.time - b.time)
    
    setPrice(close);
    setLatestCandle(candle);
  
  }, [lastJsonMessage, selectedCoin]);

  return { price, latestCandle };
};

export default useCryptoSocket;
