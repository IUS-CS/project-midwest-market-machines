#include <iostream>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXWebSocket.h>

int main() {
  ix::WebSocket demoSocket;

  std::string url("wss://stream.binance.com:9443/ws/btcusdt@trade");
  demoSocket.setUrl(url);

  demoSocket.setOnMessageCallback([](const ix::WebSocketMessagePtr &msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      std::cout << "Received: " << msg->str << std::endl;
    } else if (msg->type == ix::WebSocketMessageType::Open) {
      std::cout << "Successfully connected to Binance" << std::endl;
    } else if (msg->type == ix::WebSocketMessageType::Error) {
      std::cout << "Error: " << msg->errorInfo.reason << std::endl;
    }
  });

  demoSocket.start();

  while (true) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
  }

  return 0;
}
