/* ExchangeClientTest
 * Test file for ExchangeClient.h
 * The Simple Trade App
 *
 * ExchangeClientTest.cpp
 *
 * This test file tests the functionality of the ExchangeClient class included
 * in ExchangeClient.cpp. Using the gtest suite, we create a test fixture class,
 * then use TEST_F(TestFixtureClassName, TestName) {} to specify the tests.
 */

#include "ExchangeClient.h"
#include "gtest/gtest.h"
#include <ixwebsocket/IXWebSocketServer.h>

using namespace std;

class ExchangeClientTest : public ::testing::Test {
protected:
  static atomic<bool> MessageEmpty;
  static unique_ptr<ix::WebSocketServer> Server;
  static thread ServerThread;
  inline static nlohmann::json BinanceKline = {{"E", 1773198900017},
                                               {"e", "kline"},
                                               {"k",
                                                {{"B", "0"},
                                                 {"L", "-1"},
                                                 {"Q", "0.00000000"},
                                                 {"T", 1773198899999},
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
                                                 {"t", 1773198840000},
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
            webSocket.send(BinanceKline.dump(0));
          }
        });

    ServerThread = thread([]() { Server->listenAndStart(); });
    this_thread::sleep_for(chrono::milliseconds(2));
  }

  static void TearDownTestSuite() {
    Server->stop();

    if (ServerThread.joinable()) {
      ServerThread.join();
    }

    Server.reset();
  }
};

atomic<bool> ExchangeClientTest::MessageEmpty{true};
thread ExchangeClientTest::ServerThread;
unique_ptr<ix::WebSocketServer> ExchangeClientTest::Server = nullptr;

TEST_F(ExchangeClientTest, SubscribesTo) {
  ExchangeClient TestClient;
  TestClient.SetCallback([](const string &msg) -> void {
    if (!msg.empty()) {
      MessageEmpty = false;
    }
  });
  TestClient.Connect("ws://127.0.0.1:9999");

  this_thread::sleep_for(chrono::milliseconds(2));
  EXPECT_FALSE(MessageEmpty);
}
