import { useState, useEffect, useRef, useMemo } from 'react';
/* 
Backend websocket connection.
*/
const useCryptoSocket = (selectedCoin) => {

  //---------------- USE STATEs ---------------------------------
  const [price, setPrice] = useState(null);
  const [latestCandle, setLatestCandle] = useState(null);
  const [holdings, setHoldings] = useState([]);
  const [transactions, setTransactions] = useState([]);
  const [historicalData, setHistoricalData] = useState({});
  const [allCoinsSnapshot, setAllCoinsSnapshot] = useState({});
  //-------------------------------------------------------------

  //--------------- SOCKET URLS ---------------------------------
  const SOCKET_URL = "ws://127.0.0.1:8080";
  const DB_URL = "ws://127.0.0.1:8081";
  //-------------------------------------------------------------

  //------------------- REFS ------------------------------------
  const activeCoinRef = useRef(selectedCoin);
  const dbSocketRef = useRef(null);
  const allCoinsBuffer = useRef({});
  const historicalBuffer = useRef({});
  //-------------------------------------------------------------

  /* Keeps activeCoinRef in sync with the currently selected coin.
   * The websocket handlers use the activeCoinRef to determine the
   * currently selected coin.
   *
   * Additionally updates setPrice based on the active coin.
   *
   * 1. Set activeCoinRef.current to selectedCoin.
   * 2. Normalize selectedCoin to uppercase.
   * 3. Get history from historicalData.
   * 4. Get a map from allCoinsBuffer.
   *    4a. Sort it.
   *    4b. Get the last member.
   * 5. If we have something in allCoinsBuffer...
   *    5a. Use it: set price and candle.
   * 6. Else if we have historicalData...
   *    4a. Use it: set price and candle.
   * 7. Else...
   *    5a. Set price and candle as null.
   */
  useEffect(() => {
    activeCoinRef.current = selectedCoin;

    const coin = selectedCoin.toUpperCase();
    const history = historicalData[coin] || [];
    const liveMap = allCoinsBuffer.current[coin];

    const liveArray = liveMap ? Array.from(liveMap.values()).sort((a, b) => a.time - b.time) : [];
    const lastLive = liveArray.length > 0 ? liveArray[liveArray.length - 1] : null;

    if (lastLive) {
      setPrice(lastLive.close);
      setLatestCandle(lastLive);
    } else if (history.length > 0) {
      setPrice(history[history.length - 1].close);
      setLatestCandle(null)
    } else {
      setPrice(null);
      setLatestCandle(null);
    }
  }, [selectedCoin, historicalData]);

  /* Receives live kline data from the backend, parses it, and updates
   * the relevant refs.
   * 
   * 1. Declare a new socket.
   * 2. Determine onmessage behavior as follows:
   *    2a. Parse the event data into a JSON.
   *    2b. If that JSON does not have 'Kline', then return.
   *    2c. If the allCoinsBuffer does not have the current coin...
   *        a. Create a new map for that coin in allCoinsBuffer.
   *    2d. Get the Map for that coin, and check if a timestamp exists for it.
   *    2e. Create a new candle from the parsed JSON.
   *    2f. Set that candle onto the map.
   *    2g. If that coin is the currently selected coin...
   *        a. Set price and set latest candle.
   * 3. Cleanup : close socket.
   */
  useEffect(() => {
    const socket = new WebSocket(SOCKET_URL);

    socket.onmessage = (event) => {
      const data = JSON.parse(event.data);
      if (!data.Kline) return;

      const coin = data.Coin.toUpperCase();
      const k = data.Kline;
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
    setAllCoinsSnapshot(prev => ({ ...prev, [coin]: candle.close }));
      if (coin === activeCoinRef.current.toUpperCase()) {
        setPrice(candle.close);
        setLatestCandle(candle);
      }
    };

    return () => socket.close();
  }, []);

  /* Receives historical data from the backend, parses it, and updates
   * historicalBuffer.
   *
   * 1. Declare a new socket.
   * 2. Determine onmessage behavior as follows:
   *    2a. Parse the event data into a JSON.
   *    2b. Open a switch case on data.dataType:
   *        a. If "holding" ... set holdings.
   *        b. If "transaction" ... set transaction.
   *        c. If "historical" ...
   *           1. If the historicalBuffer does not have the current coin...
   *              a. Make an empty array for that coin.
   *           2. Create and push a new candle to historicalBuffer.
   *           3. If it's the last piece of historical data...
   *              a. Set historicalData from historicalBuffer.
   * 3. Cleanup : close socket.
   */
  useEffect(() => {
    const socket = new WebSocket(DB_URL);
    dbSocketRef.current = socket;

    socket.onmessage = (event) => {
      const data = JSON.parse(event.data);

      switch (data.dataType) {
        case "holding":
          setHoldings((prev) => [...prev, data]);
          break;
        case "transaction":
          setTransactions((prev) => [...prev, data]);
          break;
        case "historical":
          const coin = data.coin.toUpperCase();
          if (!historicalBuffer.current[coin]) historicalBuffer.current[coin] = [];

          historicalBuffer.current[coin].push({
            time: data.time,
            open: data.price,
            high: data.price,
            low: data.price,
            close: data.price
          });

          if (data.last) {
            setHistoricalData(prev => ({
              ...prev, [coin]: [...historicalBuffer.current[coin]]
            }));
          }
          break;
      }
    };
    return () => socket.close();
  }, []);

  /* const combinedHistory.
   *
   * Uses useMemo to combine any and all received live klines with historical data,
   * such that all charts have updated data, even when they are not selected, and
   * each chart is able to load all data it needs from one location.
   *
   * 1. Normalize the selectedCoin to uppercase.
   * 2. Get the histroical data for the active coin or a blank array if none.
   * 3. Get the current coin's data from the allCoinsBuffer.
   * 4. Convert Map valus into an Array, then sort.
   * 5. Return the spread of the historical data and liveData.
   * 6. Wrap all of that into a useMemo and store the result in combinedHistory.
   */
  const combinedHistory = useMemo(() => {
    const activeCoin = selectedCoin.toUpperCase();
    const csvData = historicalData[activeCoin] || [];
    const liveBuffer = allCoinsBuffer.current[activeCoin];

    const liveData = liveBuffer ? Array.from(liveBuffer.values()).sort((a, b) => a.time - b.time) : [];

    return [...csvData, ...liveData];
  }, [selectedCoin, historicalData, latestCandle]);

  /* const trade.
   *
   * Enables sending trades over the DataBase websocket.
   *
   * 1. If the websocket is open and price is not null...
   *    1a. Log to console the trade we send.
   *    1b. Send the trade.
   *    1c. If buy...
   *        a. setHoldings with the traded coin.
   *    1d. If sell...
   *        a. Remove the last traded coin from setHoldings via slice.
   */
  const trade = (type, quantity, coin = selectedCoin) => {
  if (dbSocketRef.current?.readyState === WebSocket.OPEN && price !== null) {
    const tradePrice = allCoinsSnapshot[coin.toUpperCase()] ?? price;
    const tradeData = { type, coin, price: tradePrice, quantity };


      if (type === 'buy') {
        setHoldings((prev) => [...prev, tradeData]);
      } else if (type === 'sell') {
        setHoldings(prevHoldings => prevHoldings.slice(0, -1));
      }
    }
  };
  return { price, latestCandle, holdings, transactions, historicalCandles: combinedHistory, trade, allCoinsSnapshot, allCoinsBuffer };
};
export default useCryptoSocket;
