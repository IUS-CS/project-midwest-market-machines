import useWebSocket, { ReadyState } from 'react-use-websocket';
import { useState, useEffect } from 'react';

const WebSocket = () => {
  const [price, setPrice] = useState(0);
  const SOCKET_URL = "ws://127.0.0.1:8080";



  const { sendMessage, lastJsonMessage, readyState } = useWebSocket(
    SOCKET_URL, {
    share: true,
    shouldReconnect: () => true,
    reconnectAttempts: 1000,
    reconnectInterval: 3000,
    onOpen: () => console.log('WebSocket opened'),
    onClose: () => console.log('WebSocket closed'),
    onMessage: (event) => {
      console.log('JSON received', event.data);
      const JSONData = JSON.parse(event.data);
      const coin = JSONData["coin"];
      const price = JSONData["price"];
      console.log(coin + ": " + price);
    },
  });

  const connectionStatus = {
    [ReadyState.CONNECTING]: 'Connecting',
    [ReadyState.OPEN]: 'Open',
    [ReadyState.CLOSING]: 'Closing',
    [ReadyState.CLOSED]: 'Closed',
    [ReadyState.UNINSTANTIATED]: 'UNINSTANTIATED'
  }[readyState];

  WebSocket.onMessage = (MessageEvent) => {
    const data = JSON.parse(MessageEvent.data);
    setLastMessage(data);
    onMessage?.(data);
    console.log(data);
  }

  /*
  useEffect(() => {
    if (readyState == 1) {
      sendMessage("Test send from frontend");
    }
  }, [readyState]);
  

  useEffect(() => {
    if (data != null) {
      setPrice(data);
    }
  }, [lastJsonMessage]);
  */
  return { price, sendMessage };
};

export default WebSocket;
