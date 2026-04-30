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
#include "ixwebsocket/IXNetSystem.h"
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
 * unique_ptr<ix::WebSocketServer> is used to hold the Server.
 * ServerThread holds the server's thread.
 *
 * BinanceKline is arbitraty Kline data from a connection to Binance. This is
 * real data pulled from Binance from a run of Binance_Websockets.cpp
 */
class ExchangeClientTest : public ::testing::Test {
protected:
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

  /* static void SetUpTestSuite() {...}
   *
   * Sets up the server *once* for the entirety of the ExchangeClientTest suite.
   *
   * 1. Sets a promise / future that the server is ready.
   * 2. Sets the server's socket and address.
   * 3. Sets the server's callback behavior, in this case, send the
   *      BinanceKline.
   * 4. Starts the server on its own thread (server is blocking!)
   * 5. Checks to see if the server is ready.
   * 6. Waits for up to 1s to see if the server grabs a port.
   */
  static void SetUpTestSuite() {
    ix::initNetSystem();
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

  /* static void TearDownTestSuite() {...}
   *
   * Tears down the server at the end of the ExchangeClientTest suite.
   *
   * 1. Calls to have the Server stop.
   * 2. Joins the server's thread.
   * 3. Resets the server.
   */
  static void TearDownTestSuite() {
    Server->stop();

    if (ServerThread.joinable()) {
      ServerThread.join();
    }

    Server.reset();
  }
};

/* class TestClient
 *
 * This class exists to expose the protected methods of ExchangeClient.
 * As of yet, there is only one piece that we are interested in:
 * HandleMessages().
 *
 * Since HandleMessages(), well, handles the messages for ExchangeClient, we are
 * able to inject ix::WebSocketMessage -s directly into the message handling
 * logic and track how it responds.
 *
 * Particularly useful for mocking.
 */
class TestClient : public ExchangeClient {
public:
  using ExchangeClient::HandleMessages;
};

/* class MockClientHooks
 *
 * This mocking class describes the four currently used mock methods from
 * ExchangeClient.
 */
class MockClientHooks {
protected:
  void SetUp() { ix::initNetSystem(); }

public:
  MOCK_METHOD(void, OnOpen, (), ());
  MOCK_METHOD(void, OnMessage, (const string &msg), ());
  MOCK_METHOD(void, OnError, (const string &msg), ());
  MOCK_METHOD(void, OnClose, (), ());
};

//---------------------- GLOBALS ----------------------------------------------
string stream = "ws://127.0.0.1:9999";
thread ExchangeClientTest::ServerThread;
unique_ptr<ix::WebSocketServer> ExchangeClientTest::Server = nullptr;
//-----------------------------------------------------------------------------

/* FakeMessages() {...}
 *
 * FakeMessages is a helper method that takes advantage of the
 * ix::WebSocketMessage() constructor. It allows us to build whichever
 * WebSocketMessage that we need in the moment.
 *
 * We use the defaults for e, o, c.
 */
ix::WebSocketMessagePtr FakeMessages(ix::WebSocketMessageType Type,
                                     string Content = "") {
  static string StaticString;
  StaticString = Content;

  return make_unique<ix::WebSocketMessage>(
      Type, StaticString, StaticString.size(), ix::WebSocketErrorInfo(),
      ix::WebSocketOpenInfo(), ix::WebSocketCloseInfo());
}

//------------------------ MOCKING TESTS ---------------------------------------

/* Sequence_Open_Message_Error_Close
 *
 * A mocking test - this test checks that the HandleMessages switch case works
 * for all four message types it currently supports, and that they return their
 * defined behavior in the order they are sent.
 *
 * 1. Set behavior for each message type.
 * 2. Declare a desired sequence of method calls.
 * 3. Make the method calls.
 */
TEST(ExchangeClientTestMocking, Sequence_Open_Message_Error_Close) {
  TestClient Client;
  MockClientHooks Hooks;

  Client.SetOnOpen([&]() { Hooks.OnOpen(); });
  Client.SetOnMessage([&](const string &msg) { Hooks.OnMessage(msg); });
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

/* Client_Processes_20_Opens
 *
 * A mocking test - this test checks that if 20 open messages are sent to
 * HandleMessages in quick succession, then the defined behavior is given back
 * 20 times.
 *
 * 1. Set behavior for open messages.
 * 2. Declare we expect 20 calls to OnOpen().
 * 3. Fire off 20 open messages.
 */
TEST(ExchangeClientMocking, Client_Processes_20_Opens) {
  TestClient Client;
  MockClientHooks Hooks;

  Client.SetOnOpen([&]() { Hooks.OnOpen(); });
  EXPECT_CALL(Hooks, OnOpen()).Times(20);

  for (int i = 0; i < 20; i++) {
    Client.HandleMessages(FakeMessages(ix::WebSocketMessageType::Open));
  }
};

/* Client_Processes_20_Messages
 *
 * A mocking test - this test checks that if 20 messages are sent to
 * HandleMessages in quick succession, then the defined behavior is given back
 * 20 times.
 *
 * 1. Set behavior for messages.
 * 2. Declare we expect 20 calls to OnOpen().
 * 3. Fire off 20 messages.
 */
TEST(ExchangeClientTestMocking, Client_Processes_20_Messages) {
  TestClient Client;
  MockClientHooks Hooks;

  Client.SetOnMessage([&](const string &msg) { Hooks.OnMessage(msg); });
  EXPECT_CALL(Hooks, OnMessage(testing::HasSubstr("btcusdt"))).Times(20);

  for (int i = 0; i < 20; i++) {
    Client.HandleMessages(
        FakeMessages(ix::WebSocketMessageType::Message, "{\"s\":\"btcusdt\"}"));
  }
};

/* Client_Processes_20_Closes
 *
 * A mocking test - this test checks that if 20 closes are sent to
 * HandleMessages in quick succession, then the defined behavior is given back
 * 20 times.
 *
 * 1. Set behavior for closes.
 * 2. Declare we expect 20 calls to OnClose().
 * 3. Fire off 20 closes.
 */
TEST(ExchangeClientTestMocking, Client_Processes_20_Closes) {
  TestClient Client;
  MockClientHooks Hooks;

  Client.SetOnClose([&]() { Hooks.OnClose(); });
  EXPECT_CALL(Hooks, OnClose()).Times(20);

  for (int i = 0; i < 20; i++) {
    Client.HandleMessages(FakeMessages(ix::WebSocketMessageType::Close));
  }
};

/* Client_Processes_20_Errors
 *
 * A mocking test - this test checks that if 20 errors are sent to
 * HandleMessages in quick succession, then the defined behavior is given back
 * 20 times.
 *
 * 1. Set behavior for errors.
 * 2. Declare we expect 20 calls to OnError().
 * 3. Fire off 20 errors.
 */
TEST(ExchangeClientTestMocking, Client_Processes_20_Errors) {
  TestClient Client;
  MockClientHooks Hooks;

  Client.SetOnError([&](const string &msg) { Hooks.OnError(msg); });
  EXPECT_CALL(Hooks, OnError(testing::HasSubstr(""))).Times(20);

  for (int i = 0; i < 20; i++) {
    Client.HandleMessages(FakeMessages(ix::WebSocketMessageType::Error, ""));
  }
};

//---------------------- LIVE SOCKET TESTS ------------------------------------

/* ClientSocketOpens
 *
 * This test confirms that the ExchangeClient socket is able to open, grab a
 * connection to the server, and process its open status through HandleMessages.
 *
 * 1. Declare an ExchangeClient, promise IsOpen and Future for it.
 * 2. Set the OnOpen() behavior to then set the IsOpen bool = true.
 * 3. Connect to the server.
 * 4. Wait for up to 1 second for IsOpen to be ready.
 * 5. Expect that IsOpen is true.
 */
TEST_F(ExchangeClientTest, ClientSocketOpens) {
  ExchangeClient Client;
  promise<bool> IsOpen;
  future<bool> Future = IsOpen.get_future();

  Client.SetOnOpen([&IsOpen]() { IsOpen.set_value(true); });
  Client.Connect(stream);

  ASSERT_EQ(Future.wait_for(chrono::seconds(1)), future_status::ready);
  EXPECT_TRUE(Future.get());
}

/* ClientSocketGetsMessage
 *
 * This test confirms that the ExchangeClient is able to open, grab a connection
 * to the server, and process a message through HandleMessages.
 *
 * 1. Declare an ExchangeClient, promise Received and Future for it.
 * 2. Set the Callback behavior to set Received = true if the message is
 *    non-empty.
 * 3. Connect to the server.
 * 4. Wait for up to 1 second for Received to be ready.
 * 5. Expect that Receives is true.
 */
TEST_F(ExchangeClientTest, ClientSocketGetsMessage) {
  TestClient Client;
  promise<bool> Received;
  future<bool> Future = Received.get_future();

  Client.SetOnMessage([&Received](const string &msg) {
    if (!msg.empty()) {
      Received.set_value(true);
    }
  });

  Client.Connect(stream);

  ASSERT_EQ(Future.wait_for(chrono::seconds(1)), future_status::ready);
  EXPECT_TRUE(Future.get());
}

/* ClientSocketCloses
 *
 * This test confirms that the ExchangeClient is able to open, grab a connection
 * to the server, and process a close through HandleMessages.
 *
 * 1. Declare an ExchangeClient, promise Closed and Future for it.
 * 2. Set the OnClose() behavior to update Closed = true.
 * 3. Connect to the server.
 * 4. Wait for up to 1 second for Closed to be ready.
 * 5. Assert the current state is closed.
 * 6. Expect that Closed is true.
 *
 *  TODO: Make the server send the close message. Server needs to be refactored
 * a bit to do so. Otherwise this test is no different than the mock test.
 */
TEST_F(ExchangeClientTest, ClientSocketCloses) {
  TestClient Client;
  promise<bool> Closed;
  future<bool> Future = Closed.get_future();

  Client.SetOnClose([&Closed]() { Closed.set_value(true); });
  Client.Connect(stream);
  Client.HandleMessages(FakeMessages(ix::WebSocketMessageType::Close));

  ASSERT_EQ(Future.wait_for(chrono::seconds(1)), future_status::ready);
  ASSERT_EQ(Client.GetState(), ix::ReadyState::Closed);
  EXPECT_TRUE(Future.get());
}

/* ClientSocketErrors
 *
 * This test confirms that the ExchangeClient is able to open, grab a connection
 * to the server, and process a error through HandleMessages.
 *
 * 1. Declare and ExchangeClient, promise Error and Future for it.
 * 2. Declare a promised ix::ReadyState ErrorState and Future for it.
 * 3. Set the OnError() behavior to update both Error and ErrorState.
 * 4. Connect to the server.
 * 5. Wait for up to 1 second for Error to be ready.
 * 6. Wait for up to 1 second for ErrorState to be ready.
 * 7. Expect that Error is true.
 * 8. Expect the socket is closed.
 *
 * TODO: Make the server send the error message. Same issue as above.
 */
TEST_F(ExchangeClientTest, ClientSocketErrors) {
  TestClient Client;
  promise<bool> Error;
  future<bool> Future = Error.get_future();
  promise<ix::ReadyState> ErrorState;
  future<ix::ReadyState> FutureState = ErrorState.get_future();

  Client.SetOnError([&Error, &ErrorState, &Client](const string &msg) {
    Error.set_value(true);
    ErrorState.set_value(Client.GetState());
  });
  Client.Connect(stream);
  Client.HandleMessages(FakeMessages(ix::WebSocketMessageType::Error, ""));

  ASSERT_EQ(Future.wait_for(chrono::seconds(1)), future_status::ready);
  ASSERT_EQ(FutureState.wait_for(chrono::seconds(1)), future_status::ready);
  EXPECT_TRUE(Future.get());
  EXPECT_EQ(FutureState.get(), ix::ReadyState::Closed);
}
