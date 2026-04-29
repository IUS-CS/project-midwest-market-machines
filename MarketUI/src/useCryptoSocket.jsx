import { useState, useEffect, useRef } from 'react';
/* 
Backend websocket connection.
*/
const useCryptoSocket = (selectedCoin) => {
  const [price, setPrice] = useState(null);
  const [latestCandle, setLatestCandle] = useState(null);

  const SOCKET_URL = "ws://127.0.0.1:8080";
  const DB_URL = "ws://127.0.0.1:8081";

  const candleMapRef = useRef(new Map());
  const lastCloseRef = useRef(null);
  const prevCoinRef = useRef(null);

  const [holdings, setHoldings] = useState([]);
  const [transactions, setTransactions] = useState([]);
  const [historicalData, setHistoricalData] = useState({});
  const dbSocketRef = useRef(null);
  const [lastJsonMessage, setLastJsonMessage] = useState(null);

  const [combinedHistory, setCombinedHistory] = useState([]);
  const historicalBuffer = useRef({});
  const allCoinsBuffer = useRef({});
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

    socket.onmessage = (event) => {
      const jsonReceived = JSON.parse(event.data);
      setLastJsonMessage(jsonReceived);
    }
    return () => socket.close();
  }, []);

  useEffect(() => {
    const socket = new WebSocket(DB_URL);
    dbSocketRef.current = socket;
    socket.onmessage = (event) => {
      const data = JSON.parse(event.data);

      if (data.dataType === "holding") {
        setHoldings((prev) => [...prev, data]);
        return;
      }

      if (data.dataType === "transaction") {
        setTransactions((prev) => [...prev, data]);
        return;
      }

      if (data.dataType === "historical") {
        const coinupper = data.coin.toUpperCase();

        if (!historicalBuffer.current[coinupper]) historicalBuffer.current[coinupper] = [];

        historicalBuffer.current[coinupper].push({
          time: data.time,
          open: data.price,
          high: data.price,
          low: data.price,
          close: data.price
        });

        if (data.last) {
          setHistoricalData((prev) => ({
            ...prev,
            [coinupper]: [...historicalBuffer.current[coinupper]]
          }));
        }
        return;
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
    if (!lastJsonMessage || !lastJsonMessage.Kline) { return; }

    const coin = lastJsonMessage.Coin.toUpperCase();
    const activeCoin = selectedCoin.toUpperCase();
    const k = lastJsonMessage.Kline;
    const timeSeconds = Math.floor(k.StartTime / 1000);

    if (!allCoinsBuffer.current[coin]) {
      allCoinsBuffer.current[coin] = new Map();
    }

    const coinMap = allCoinsBuffer.current[coin];
    const existing = coinMap.get(timeSeconds);

    const candle = {
      time: timeSeconds,
      open: existing ? existing.open : parseFloat(k.Open),
      high: Math.max(parseFloat(k.High), existing ? existing.high : 0),
      low: Math.min(parseFloat(k.Low), existing ? existing.low : Infinity),
      close: parseFloat(k.Close),
    };

    coinMap.set(timeSeconds, candle);

    if (coin === activeCoin) {
      setPrice(candle.close);
      setLatestCandle({ ...candle });
    }

  }, [lastJsonMessage, selectedCoin]);

  const trade = (type, quantity) => {
    if (dbSocketRef.current?.readyState === WebSocket.OPEN) {
      if (price === null) {
        console.warn("No price yet, trade blocked.");
        return;
      }
      if (!quantity || quantity <= 0) {
        console.warn("Invalid quantity, trade blocked.");
        return;
      }
      const tradeData = {
        type: type,
        coin: selectedCoin,
        price: price,
        quantity: quantity
      };

      if (type === 'buy') {
        setHoldings((prev) => [...prev, tradeData]);
      } else if (type === 'sell') {
        setHoldings(prevHoldings => prevHoldings.slice(0, -1));
      }
      console.log("Sending to backend:", tradeData);
      dbSocketRef.current.send(JSON.stringify(tradeData));
    }
  };

  useEffect(() => {
    const activeCoin = selectedCoin.toUpperCase();
    const csvData = historicalData[activeCoin] || [];
    const liveBuffer = allCoinsBuffer.current[activeCoin];

    const liveData = liveBuffer
      ? Array.from(liveBuffer.values()).sort((a, b) => a.time - b.time) : [];

    const combined = [...csvData, ...liveData];
    setCombinedHistory(combined);

    if (combined.length > 0) {
      const lastPrice = combined[combined.length - 1].close;
      setPrice(prev => (typeof prev === 'number' ? prev : lastPrice));
    } else {
      setPrice("Loading...");
    }
  }, [selectedCoin, historicalData]);

  return { price, latestCandle, holdings, transactions, historicalCandles: combinedHistory, trade };
};
export default useCryptoSocket;
