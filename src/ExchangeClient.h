/* ExchangeClient
 * Class header file.
 * The Simple Trade App
 *
 * ExchangeClient.h
 *
 * This header file creates the ExchangeClient class.
 * The ExchangeClient class creates private members:
 *    1. A websocket.
 *    2. A Processor (see BinanceProcessor.cpp).
 *    3. An executable callback for event handling (OnCallback).
 *    4. A mutex for printing to console, if desired.
 *    5. A debug flag (bool).
 *
 * Then, as public methods:
 *    1. void setDEBUG(bool debug)
 *        - true/false for DEBUG flag.
 *    2. void SetCallback(function<void(const string &)> Behavior)
 *        - Pass the behavior function you'd like into the class
 *          for what the ExchangeClient should do with the message on receipt.
 *    3. void Connect(string stream)
 *        - Point the ExchangeClient to the stream you'd like, and start it.
 *
 * ExchangeClient uses ix::IXWebSocket.h to create a client websocket.
 * It uses the non-blocking call `socket.start()`.
 */

#pragma once

#include "BinanceProcessor.h"
#include "ixwebsocket/IXWebSocketMessage.h"
#include "ixwebsocket/IXWebSocketMessageType.h"
#include <functional>
#include <iostream>
#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>

using WebSocket = ix::WebSocket;
using json = nlohmann::json;
using namespace std;

class ExchangeClient {
protected:
  void HandleMessages(const ix::WebSocketMessagePtr &msg) {
    switch (msg->type) {
    case ix::WebSocketMessageType::Open:
      if (OnOpen) {
        OnOpen();
      }
      break;
    case ix::WebSocketMessageType::Message:
      if (OnCallback) {
        OnCallback(msg->str);
      }
      if (DEBUG) {
        PrintLocker.lock();
        cout << "Received Message:\n" << msg->str << "\n" << endl;
        PrintLocker.unlock();
      }
      break;
    case ix::WebSocketMessageType::Close:
      if (OnClose) {
        OnClose();
      }
      socket.close();
      break;
    case ix::WebSocketMessageType::Error:
      if (OnError) {
        OnError(msg->errorInfo.reason);
      }
      break;
    default:
      break;
    }
  }

private:
  WebSocket socket;
  function<void(const string &)> OnCallback;
  function<void()> OnOpen;
  function<void()> OnClose;
  function<void(const string &)> OnError;
  mutex PrintLocker;
  bool DEBUG = false;

public:
  void setDEBUG(bool debug) { DEBUG = debug; }

  void SetCallback(function<void(const string &)> Behavior) {
    OnCallback = Behavior;
  }

  void SetOnOpen(function<void()> Behavior) { OnOpen = Behavior; }

  void SetOnClose(function<void()> Behavior) { OnClose = Behavior; }

  void SetOnError(function<void(const string &)> Behavior) {
    OnError = Behavior;
  }

  void Connect(string stream) {
    socket.setUrl(stream);

    if (DEBUG) {
      cout << "Subscribed to: " << stream << endl;
    }

    socket.setOnMessageCallback(
        [this](const ix::WebSocketMessagePtr &msg) -> void {
          HandleMessages(msg);
        });
    socket.start();
  }
};
