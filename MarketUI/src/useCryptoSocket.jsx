import { useState, useEffect, useRef } from 'react';

/* 
Backend websocket connection.
*/
const useCryptoSocket = (selectedCoin) => {
  const [price, setPrice] = useState(null);
  const [latestCandle, setLatestCandle] = useState(null);
  const [lastJsonMessage, setLastJsonMessage] = useState(null);
  const [holdings, setHoldings] = useState([]);

  const SOCKET_URL = "ws://127.0.0.1:8080";

  const candleMapRef = useRef(new Map());
  const lastCloseRef = useRef(null);
  const prevCoinRef = useRef(null);

  /* useEffect() for WebSocket socket.
   *
   * 1. Define WebSocket socket with SOCKET_URL.
   * 2. Set bool isStartup = true.
   * 2. Define the socket's onmessage behavior.
   *    2a. Parse the received JSON into jsonReceived
   *    2b. Check if isStartup.
   *        - setHoldings with received data.
   *        - Set isStartup = false if last = true.
   *    2c. If it's a kline...
   *        - setLastJsonMessage with the received JSON.
   * 3. Close the socket when finished.
   *
   * '[]' set to run this useEffect only once on startup.
   *
   * More information on this WebSocket API can be found at:
   * https://developer.mozilla.org/en-US/docs/Web/API/WebSockets_API
  */
  useEffect(() => {
    const socket = new WebSocket(SOCKET_URL);

    let isStartup = true;

    socket.onmessage = (event) => {
      const jsonReceived = JSON.parse(event.data);

      /* For startup, we expect this:
       *
       * {
       *    coin: <coin>,
       *    quantity: <quantity>,
       *    time: <UNIX time>,
       *    price: <price>,
       *    last: <bool : is last?>
       * }
       */
      if (isStartup) {
        setHoldings((prev) => [...prev, {
          coin: data.coin,
          quantity: data.quantity,
          time: data.time,
          price: data.price
        }]);

        if (data.last === true) {
          isStartup = false;
          console.log("Got last holding.");
        }
        return;
      }

      if (data.Kline) {
        setLastJsonMessage(jsonReceived);
      }
    };

    return () => socket.close();
  }, []);

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
    if (!lastJsonMessage || lastJsonMessage.Coin !== selectedCoin || !lastJsonMessage.Kline) { return; }

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
