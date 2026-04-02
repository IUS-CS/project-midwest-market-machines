/* ExchangeClient
 * Class header file.
 * The Simple Trade App
 *
 * ExchangeClient.h
 *
 * This header file creates the ExchangeClient class.
 * The ExchangeClient class contains a protected method:
 *    1. HandleMessages(const ix::WebSocketMessagePtr &msg)
 *        - Defines which function to call on which message type.
 *
 * The ExchangeClient class contains these private members:
 *    1. A WebSocket.
 *    2. A function<void(const string &)> OnCallback.
 *    3. A function<void()> OnOpen.
 *    4. A function<void()> OnClose.
 *    5. A function<void(const string &)> OnError.
 *    6. A mutex PrintLocker.
 *    7. A bool DEBUG.
 *
 * Then, as public methods:
 *    1. ix::ReadyState GetState()
 *        - A getter for the ExchangeClient's state.
 *    2. void SetDEBUG(bool debug)
 *        - A setter for bool DEBUG.
 *        - == true for debug prints. false if not.
 *    3.  void SetCallback(function<void(const string &)> Behavior)
 *        - Pass the behavior function you'd like for what ExchangeClient should
 *          do with a message type message on receipt.
 *    4. void SetOnOpen(function<void()> Behavior)
 *        - Pass the behavior function you'd like for what ExchangeClient should
 *          do with a message type open on receipt.
 *    5. void SetOnClose(function<void()> Behavior)
 *        - Pass the behavior function you'd like for what ExchangeClient should
 *          do with a message type close on receipt.
 *    6. void SetOnError(function<void(const string &msg)> Behavior)
 *        - Pass the behavior function you'd like for what ExchangeClient should
 *          do with a message type error on receipt.
 *    7. void Connect(string stream)
 *        - Point the ExchangeClient to the stream you'd like, and start it.
 *
 * ExchangeClient uses ix::IXWebSocket.h to create a client websocket.
 * It uses the non-blocking call `socket.start()`.
 */

#pragma once

#include "ixwebsocket/IXWebSocketMessage.h"
#include "ixwebsocket/IXWebSocketMessageType.h"
#include <functional>
#include <iostream>
#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>

using WebSocket = ix::WebSocket;
using json = nlohmann::json;
using MessagePtr = ix::WebSocketMessagePtr;
using MessageType = ix::WebSocketMessageType;
using namespace std;

class ExchangeClient {
protected:
  /* void HandleMessages(const ix::WebSocketMessagePtr &msg) {...}
   *
   * HandleMessages exists (and is marked protected) to expose the
   * MessageType::<?> logic to subclasses, primarily for the purposes of
   * testing.
   *
   * For each type of message we care about, we make a call to the defined
   * OnType() function.
   */
  void HandleMessages(const MessagePtr &msg) {
    switch (msg->type) {
    case MessageType::Open:
      if (OnOpen) {
        OnOpen();
      }
      break;
    case MessageType::Message:
      if (OnCallback) {
        OnCallback(msg->str);
      }
      if (DEBUG) {
        PrintLocker.lock();
        cout << "Received Message:\n" << msg->str << "\n" << endl;
        PrintLocker.unlock();
      }
      break;
    case MessageType::Close:
      if (OnClose) {
        OnClose();
      }
      socket.close();
      break;
    case MessageType::Error:
      if (OnError) {
        OnError(msg->errorInfo.reason);
      }
      break;
    default:
      break;
    }
  }

private:
  /* Variables and Objects:
   *
   * 1. WebSocket socket;
   *    - An IXWebSocket client.
   * 2. function<void(const string &)> OnCallback
   *    - A function to define behavior on message type message.
   * 3. function<void()> OnOpen.
   *    - A function to define behavior on message type open.
   * 4. function<void()> OnClose.
   *    - A function to define behavior on message type close.
   * 5. function<void()> OnError.
   *    - A function to define behavior on message type error.
   * 6. mutex PrintLocker.
   *    - A mutex used to lock calls to `cout` so DEBUG prints don't get jumbled
   *      across the various ExchangeClients.
   * 7. bool DEBUG.
   *    - DEBUG == true if you'd like DEBUG prints to console.
   *    - DEBUG == false if not (default).
   */
  WebSocket socket;
  function<void(const string &)> OnCallback;
  function<void()> OnOpen;
  function<void()> OnClose;
  function<void(const string &)> OnError;
  mutex PrintLocker;
  bool DEBUG = false;

public:
  /* ix::ReadyState getState() {..}
   *
   * A getter method for WebSocket socket's ready state.
   * Returns an ix::ReadyState object, which should be part of an enum.
   *
   * The following is from the definition in IXWebSocket.h:
   *
   https://developer.mozilla.org/en-US/docs/Web/API/WebSocket#Ready_state_constants
   * enum class ReadyState
   * {
   *     Connecting = 0,
   *     Open = 1,
   *     Closing = 2,
   *     Closed = 3
   * };
   */
  ix::ReadyState GetState() { return socket.getReadyState(); }

  void SetDEBUG(bool debug) { DEBUG = debug; }

  void SetCallback(function<void(const string &)> Behavior) {
    OnCallback = Behavior;
  }

  void SetOnOpen(function<void()> Behavior) { OnOpen = Behavior; }

  void SetOnClose(function<void()> Behavior) { OnClose = Behavior; }

  void SetOnError(function<void(const string &)> Behavior) {
    OnError = Behavior;
  }

  /* void Connect(string stream) {...}
   *
   * A helper method for connect an ExchangeClient to a WebSocket stream.
   *
   * 1. Sets the socket URL from string stream.
   * 2. Prints stream to console if DEBUG == true.
   * 3. Set's socket's behavior to use protected HandleMessages.
   * 4. Starts the socket.
   *    - Asynchronous call. Check definition for more info.
   */
  void Connect(string stream) {
    socket.setUrl(stream);

    if (DEBUG) {
      cout << "Subscribed to: " << stream << endl;
    }

    socket.setOnMessageCallback(
        [this](const MessagePtr &msg) -> void { HandleMessages(msg); });
    socket.start();
  }
};
