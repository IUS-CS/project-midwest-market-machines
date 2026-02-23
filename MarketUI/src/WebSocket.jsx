import useWebSocket from 'react-use-websocket';
import { useState, useEffect } from 'react';

const WebSocket = () => {
  const [price, setPrice] = useState(0);
  const SOCKET_URL = "ws://127.0.0.1:8080";

  const { sendMessage, lastMessage, readyState } = useWebSocket(
    SOCKET_URL, {
    share: false,
    shouldReconnect: () => true,
  });

  useEffect(() => {
    if (readyState == 1) {
      sendMessage("Test send from frontend");
    }
  }, [readyState]);

  useEffect(() => {
    if (lastMessage) {
      setPrice(lastMessage);
    }
  }, [lastMessage]);

  return { price, sendMessage };
};

export default WebSocket;
