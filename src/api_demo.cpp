#include <iostream>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>

int main() {

  nlohmann::json demojson = {
      {"id", 1},
      {"method", "SUBSCRIBE"},
      {"params", {"btcusdt@trade"}},
  };

  ix::WebSocket demoSocket;

  std::string url("wss://stream.binance.us:9443/ws/btcusdt@trade");
  demoSocket.setUrl(url);

  demoSocket.setOnMessageCallback([&demoSocket, &demojson](
                                      const ix::WebSocketMessagePtr &msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      auto data = nlohmann::json::parse(msg->str);
      if (data.contains("p")) {
        // auto data = nlohmann::json::parse(msg->str);
        std::string price = data["p"];
        std::string quantity = data["q"];
        std::cout << "Received: " << msg->str << std::endl;
      } else {
        std::cout << "Return: " << msg->str << std::endl;
      }
    } else if (msg->type == ix::WebSocketMessageType::Open) {
      std::cout << "Successfully connected to Binance. Please wait for data to "
                   "be sent."
                << std::endl;
      demoSocket.send(demojson.dump());
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
