/*
 *
 */

#include "ExchangeClient.h"
#include "ixwebsocket/IXConnectionState.h"
#include "ixwebsocket/IXWebSocket.h"
#include "ixwebsocket/IXWebSocketMessage.h"
#include "ixwebsocket/IXWebSocketMessageType.h"
#include "gtest/gtest.h"
#include <cstddef>
#include <ixwebsocket/IXWebSocketServer.h>
#include <thread>
#include <unistd.h>

using namespace std;

class ExchangeClientTest : public ::testing::Test {
protected:
  static unique_ptr<ix::WebSocketServer> Server;
  static thread ServerThread;
  inline static nlohmann::json BinanceKline = {{"E", "1773198900017"},
                                               {"e", "kline"},
                                               {"k",
                                                {{"B", "0"},
                                                 {"L", "-1"},
                                                 {"Q", "0.00000000"},
                                                 {"T", "1773198899999"},
                                                 {"V", "0.00000000"},
                                                 {"c", "2023.78000000"},
                                                 {"f", "-1"},
                                                 {"h", "2023.78000000"},
                                                 {"i", "1m"},
                                                 {"l", "2023.78000000"},
                                                 {"n", 0},
                                                 {"o", "2023.78000000"},
                                                 {"q", "0.00000000"},
                                                 {"s", "ETHUSDT"},
                                                 {"t", "1773198840000"},
                                                 {"v", "0.00000000"},
                                                 {"x", true}}},
                                               {"s", "ETHUSDT"}};

  static void SetUpTestSuite() {
    Server = std::make_unique<ix::WebSocketServer>(9999, "127.0.0.1");

    Server->setOnClientMessageCallback(
        [BinanceKline =
             BinanceKline](std::shared_ptr<ix::ConnectionState> connectionState,
                           ix::WebSocket &webSocket,
                           const ix::WebSocketMessagePtr &msg) -> void {
          if (msg->type == ix::WebSocketMessageType::Open) {
            webSocket.send(BinanceKline);
          }
        });

    ServerThread = thread([]() { Server->listenAndStart(); });
  }

  static void TearDownTestSuite() {
    Server->stop();
    ServerThread.join();
    Server.reset();
  }
};

thread ExchangeClientTest::ServerThread;
unique_ptr<ix::WebSocketServer> ExchangeClientTest::Server = nullptr;

TEST_F(ExchangeClientTest, SubscribesTo) {
  bool MessageEmpty = true;

  ExchangeClient TestClient;
  TestClient.SetCallback([&MessageEmpty](const string &msg) -> void {
    if (msg.empty()) {
      MessageEmpty = true;
    } else {
      MessageEmpty = false;
    }
  });
  TestClient.Connect("wss://127.0.0.1:9999");

  // sleep(1);
  EXPECT_FALSE(MessageEmpty);
}
