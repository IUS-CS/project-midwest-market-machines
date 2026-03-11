#pragma once

#include "BinanceProcessor.h"
#include "ixwebsocket/IXWebSocketMessage.h"
#include <functional>
#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>

using WebSocket = ix::WebSocket;
using json = nlohmann::json;
using namespace std;

class ExchangeClient {
private:
  WebSocket socket;
  BinanceProcessor Processor;
  function<void(const string &)> OnCallback;

public:
  void SetCallback(function<void(const string &)> Behavior) {
    OnCallback = Behavior;
  }

  void Connect(string stream) {
    socket.setUrl(stream);
    socket.setOnMessageCallback(
        [this](const ix::WebSocketMessagePtr &msg) -> void {
          if (msg->type == ix::WebSocketMessageType::Message) {
            json received = json::parse(msg->str);

            json shortened = Processor.toSimpleKline(received);
            string outbound = shortened.dump(0);

            if (OnCallback) {
              OnCallback(shortened.dump(0));
            }
          } else if (msg->type == ix::WebSocketMessageType::Close) {
            socket.close();
          }
        });
    socket.start();
  }
};
