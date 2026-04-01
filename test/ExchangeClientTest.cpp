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
#include "ixwebsocket/IXWebSocket.h"
#include "ixwebsocket/IXWebSocketErrorInfo.h"
#include "ixwebsocket/IXWebSocketMessage.h"
#include "ixwebsocket/IXWebSocketMessageType.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <future>
#include <gmock/gmock.h>
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
    promise<bool> IsReady;
    future<bool> FutureServerReady = IsReady.get_future();

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
    IsReady.set_value(Server->getPort());
    FutureServerReady.wait_for(chrono::seconds(1));
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

class MockClientHooks {
public:
  MOCK_METHOD(void, OnOpen, (), ());
  MOCK_METHOD(void, OnMessage, (const string &msg), ());
  MOCK_METHOD(void, OnError, (const string &msg), ());
  MOCK_METHOD(void, OnClose, (), ());
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

TEST(ExchangeClientTestMocking, Sequence_Open_Message_Error_Close) {
  TestClient Client;
  MockClientHooks Hooks;

  Client.SetOnOpen([&]() { Hooks.OnOpen(); });
  Client.SetCallback([&](const string &msg) { Hooks.OnMessage(msg); });
  Client.SetOnError([&](const string &msg) { Hooks.OnError(msg); });
  Client.SetOnClose([&]() { Hooks.OnClose(); });

  {
    testing::InSequence Sequence;
    EXPECT_CALL(Hooks, OnOpen());
    EXPECT_CALL(Hooks, OnMessage(testing::HasSubstr("btcusdt")));
    EXPECT_CALL(Hooks, OnError(testing::HasSubstr("")));
    EXPECT_CALL(Hooks, OnClose());
  }

  Client.HandleMessages(FakeMessages(ix::WebSocketMessageType::Open));
  Client.HandleMessages(
      FakeMessages(ix::WebSocketMessageType::Message, "{\"s\":\"btcusdt\"}"));
  Client.HandleMessages(FakeMessages(ix::WebSocketMessageType::Error, ""));
  Client.HandleMessages(FakeMessages(ix::WebSocketMessageType::Close));
};

TEST(ExchangeClientMocking, Client_Processes_20_Opens) {
  TestClient Client;
  MockClientHooks Hooks;

  Client.SetOnOpen([&]() { Hooks.OnOpen(); });
  EXPECT_CALL(Hooks, OnOpen()).Times(20);

  for (int i = 0; i < 20; i++) {
    Client.HandleMessages(FakeMessages(ix::WebSocketMessageType::Open));
  }
};

TEST(ExchangeClientTestMocking, Client_Processes_20_Messages) {
  TestClient Client;
  MockClientHooks Hooks;

  Client.SetCallback([&](const string &msg) { Hooks.OnMessage(msg); });
  EXPECT_CALL(Hooks, OnMessage(testing::HasSubstr("btcusdt"))).Times(20);

  for (int i = 0; i < 20; i++) {
    Client.HandleMessages(
        FakeMessages(ix::WebSocketMessageType::Message, "{\"s\":\"btcusdt\"}"));
  }
};

TEST(ExchangeClientTestMocking, Client_Processes_20_Closes) {
  TestClient Client;
  MockClientHooks Hooks;

  Client.SetOnClose([&]() { Hooks.OnClose(); });
  EXPECT_CALL(Hooks, OnClose()).Times(20);

  for (int i = 0; i < 20; i++) {
    Client.HandleMessages(FakeMessages(ix::WebSocketMessageType::Close));
  }
};

TEST(ExchangeClientTestMocking, Client_Processes_20_Errors) {
  TestClient Client;
  MockClientHooks Hooks;

  Client.SetOnError([&](const string &msg) { Hooks.OnError(msg); });
  EXPECT_CALL(Hooks, OnError(testing::HasSubstr(""))).Times(20);

  for (int i = 0; i < 20; i++) {
    Client.HandleMessages(FakeMessages(ix::WebSocketMessageType::Error, ""));
  }
};

TEST_F(ExchangeClientTest, ClientSocketOpens) {
  ExchangeClient TestClient;
  promise<bool> IsOpen;
  future<bool> future = IsOpen.get_future();

  TestClient.SetOnOpen([&IsOpen]() { IsOpen.set_value(true); });
  TestClient.Connect(stream);

  ASSERT_EQ(future.wait_for(chrono::seconds(1)), future_status::ready);
  EXPECT_TRUE(future.get());
}

TEST_F(ExchangeClientTest, ClientSocketGetsMessage) {
  TestClient Client;
  promise<bool> Received;
  future<bool> FutureReceived = Received.get_future();

  Client.SetCallback([&Received](const string &msg) {
    if (!msg.empty()) {
      Received.set_value(true);
    }
  });
}
