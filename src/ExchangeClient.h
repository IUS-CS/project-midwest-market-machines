#pragma once

#include "BinanceProcessor.h"
#include "ixwebsocket/IXWebSocketMessage.h"
#include <functional>
#include <iostream>
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
  mutex PrintLocker;
  bool DEBUG = false;

public:
  void setDEBUG(bool debug) { DEBUG = debug; }

  void SetCallback(function<void(const string &)> Behavior) {
    OnCallback = Behavior;
  }

  void Connect(string stream) {
    socket.setUrl(stream);

    if (DEBUG) {
      cout << "Subscribed to: " << stream << endl;
    }

    socket.setOnMessageCallback(
        [this](const ix::WebSocketMessagePtr &msg) -> void {
          if (msg->type == ix::WebSocketMessageType::Message) {
            json received = json::parse(msg->str);

            json shortened = Processor.toSimpleKline(received);
            string outbound = shortened.dump(0);

            if (OnCallback) {
              OnCallback(shortened.dump(0));
            }

            if (DEBUG) {
              PrintLocker.lock();
              cout << "Received:\n" << received.dump(2) << "\n" << endl;
              cout << "Sent to client:\n" << outbound << "\n" << endl;
              cout << "----------------------------------------" << endl;
              PrintLocker.unlock();
            }
          } else if (msg->type == ix::WebSocketMessageType::Close) {
            socket.close();
          }
        });
    socket.start();
  }
};
