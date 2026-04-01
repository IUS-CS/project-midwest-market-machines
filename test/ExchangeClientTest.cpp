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
#include "ixwebsocket/IXWebSocketErrorInfo.h"
#include "ixwebsocket/IXWebSocketMessage.h"
#include "gtest/gtest.h"
#include <ixwebsocket/IXWebSocketServer.h>

using namespace std;

/* class ExchangeClientTest
 *
 * This class holds data items that will be used *throughout* the test suites.
 * In accordance with gtest's guides, this class is derived from Test.
 *
 * Convention is to use SetUp() and TearDown() to describe the setup and
 * teardown processes, however, this sets up and tears down *between* each test.
 * We use SetUpTestSuite() and TearDownTestSuite() to setup once for the entire
 * suite.
 *
 * atomic<bool> MessageEmpty is used in the first test. It should be atomic<> as
 * SetCallback is non-blocking. Possible race condition unless the bool state is
 * shared across threads.
 *
 * unique_ptr<ix::WebSocketServer> is used to hold the Server.
 * ServerThread holds the server's thread.
 *
 * BinanceKline is arbitraty Kline data from a connection to Binance. This is
 * real data pulled from Binance from a run of Binance_Websockets.cpp
 */
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
    // Wait 2ms for the server to warm up (grab a port).
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

class TestClient : public ExchangeClient {
public:
  using ExchangeClient::HandleMessages;
};

string stream = "ws://127.0.0.1:9999";
atomic<bool> ExchangeClientTest::MessageEmpty{true};
thread ExchangeClientTest::ServerThread;
unique_ptr<ix::WebSocketServer> ExchangeClientTest::Server = nullptr;

ix::WebSocketMessagePtr FakeMessages(ix::WebSocketMessageType Type,
                                     string Content = "") {
  static string StaticString;
  StaticString = Content;

  return make_unique<ix::WebSocketMessage>(
      Type, StaticString, StaticString.size(), ix::WebSocketErrorInfo(),
      ix::WebSocketOpenInfo(), ix::WebSocketCloseInfo());
}

TEST_F(ExchangeClientTest, ClientSocketOpens) {
  ExchangeClient TestClient;
  bool isOpen = false;
  TestClient.SetOnOpen([&isOpen]() { isOpen = true; });

  TestClient.Connect(stream);

  this_thread::sleep_for(chrono::milliseconds(2));

  EXPECT_TRUE(isOpen);
}

TEST(ExchangeClientLogic, OnOpen_Logic_Fires) {
  TestClient Client;
  bool OnOpen_Fired = false;

  Client.SetOnOpen([&OnOpen_Fired]() { OnOpen_Fired = true; });

  auto FakeOpen = FakeMessages(ix::WebSocketMessageType::Open);
  Client.HandleMessages(FakeOpen);

  EXPECT_TRUE(OnOpen_Fired);
}

TEST_F(ExchangeClientTest, GetsMessage) {
  ExchangeClient TestClient;
  TestClient.SetCallback([](const string &msg) -> void {
    // If msg is not empty, then we received something.
    if (!msg.empty()) {
      MessageEmpty = false;
    }
  });
  TestClient.Connect(stream);

  // Wait 2ms for any funny business in the client thread to finish.
  this_thread::sleep_for(chrono::milliseconds(2));
  EXPECT_FALSE(MessageEmpty);
}

TEST_F(ExchangeClientTest, t){};
