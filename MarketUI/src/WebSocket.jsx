import useWebSocket, { ReadyState } from 'react-use-websocket';
import { useState } from 'react';

/* 
Backend websocket connection.
*/
const WebSocket = (selectedCoin) => {
  const [price, setPrice] = useState(0);
  const SOCKET_URL = "ws://127.0.0.1:8080";



  const { sendMessage, readyState } = useWebSocket(
    SOCKET_URL, {
    share: true,
    shouldReconnect: () => true,
    reconnectAttempts: 1000,
    reconnectInterval: 3000,
    onOpen: () => console.log('WebSocket opened'),
    onClose: () => console.log('WebSocket closed'),
    onMessage: (event) => {
      /* JSON parsing, console logging, and string interpretation*/
      console.log('JSON received', event.data);
      const JSONData = JSON.parse(event.data);
      const coin = JSONData["Coin"];
      const price = JSONData["Kline"]?.["Close"];
      console.log(coin + ": $" + price);
      /* Filter logic to listen to changes in only selected coin*/
      if (coin === selectedCoin) {
        setPrice(price);
      }
    },
  });
  return { price, sendMessage };
};

export default WebSocket;
