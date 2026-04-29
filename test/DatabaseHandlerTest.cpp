/* DatabaseHandlerTest
 * Test file for Databade_Handler.h
 * The Simple Trade App
 *
 * DatabaseHandlerTest.cpp
 *
 *
 */

#include "Database_Handler.h"
#include "ixwebsocket/IXConnectionState.h"
#include "ixwebsocket/IXWebSocketMessage.h"
#include "ixwebsocket/IXWebSocketServer.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <future>
#include <gmock/gmock.h>

using namespace std;

class Database_HandlerTest : public ::testing::Test {
protected:
  static unique_ptr<ix::WebSocketServer> Server;
  static thread ServerThread;

  static void SetUpTestSuite() {
    promise<bool> IsReady;
    future<bool> FutureServerReady = IsReady.get_future();

    Server = make_unique<ix::WebSocketServer>(9999, "127.0.0.1");

    Server->setOnClientMessageCallback(
        [](shared_ptr<ix::ConnectionState> connectionState,
           ix::WebSocket &webSocket,
           const ix::WebSocketMessagePtr &msg) -> void {
          if (NULL) {
          }
        });
  }
};
