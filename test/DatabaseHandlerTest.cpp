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
#include "ixwebsocket/IXWebSocket.h"
#include "ixwebsocket/IXWebSocketMessage.h"
#include "ixwebsocket/IXWebSocketMessageType.h"
#include "ixwebsocket/IXWebSocketServer.h"
#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <filesystem>
#include <future>
#include <gmock/gmock.h>

using namespace std;

/* class Database_HandlerTest
 *
 * This class holds data items that will be used *throughout* the test suites.
 * In accordance with gtest's guidelines, this class is derived from Test.
 *
 * We use SetUpTestSuite() and TearDownTestSuite() to setup / teardown *once*
 * for the entire suite.
 *
 * unique_ptr<ix::WebSocketServer> is used to hold the Server.
 * ServerThread holds the Server's thread.
 */
class Database_HandlerTest : public ::testing::Test {
protected:
  static unique_ptr<ix::WebSocketServer> Server;
  static thread ServerThread;

  /* static void SetUpTestSuite() {...}
   *
   * Sets up the server *once* for the test suite.
   *
   * 1. Sets a promise / future that the server is ready.
   * 2. Sets the server's socket and address.
   * 3. Sets the server's callback behavior.
   * 4. Starts the server on its own thread (server is blocking!)
   * 5. Checks to see if the server is ready.
   * 6. Waits for up to 1s to see if the server grabs a port.
   */
  static void SetUpTestSuite() {
    promise<bool> IsReady;
    future<bool> FutureServerReady = IsReady.get_future();

    Server = make_unique<ix::WebSocketServer>(9999, "127.0.0.1");

    Server->setOnClientMessageCallback(
        [](shared_ptr<ix::ConnectionState> connectionState,
           ix::WebSocket &webSocket,
           const ix::WebSocketMessagePtr &msg) -> void {
          Database_Handler db;

          if (msg->type == ix::WebSocketMessageType::Open) {
          }
        });

    ServerThread = thread([]() { Server->listenAndStart(); });
    IsReady.set_value(Server->getPort());
    FutureServerReady.wait_for(chrono::seconds(1));
  }

  /* static void TearDownTestSuite() {...}
   *
   * Tears down the server at the end of the test suite.
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

/* class MockDatabaseHooks
 *
 * This mocking class describes the four currently used mock methods from
 * Database_Handler.
 */
class MockDatabaseHooks {
public:
  MOCK_METHOD(void, recordTransaction, (json * transactionData), ());
  MOCK_METHOD(void, sendHoldingsData, (ix::WebSocket & webSocket), ());
  MOCK_METHOD(void, sendTransactionHistory, (ix::WebSocket & webSocket), ());
  MOCK_METHOD(void, sendHistoricalData,
              (ix::WebSocket & webSocket, const string &coin), ());
};

//----------------------- GLOBALS ------------------------------------------
int port = 9999;
string host = "127.0.0.1";

/*
 */
TEST(Database_Handler, Record_A_Transaction) {
  Database_Handler Database("./testData/");
  MockDatabaseHooks Hooks;
  json Transaction, Found;

  Transaction["type"] = "buy";
  Transaction["coin"] = "BTCUSDT";
  Transaction["quantity"] = 1.00;

  EXPECT_CALL(Hooks, recordTransaction(&Transaction));

  Database.recordTransaction(&Transaction);

  ifstream file("./testData/transactionHistory.csv");
  if (file.is_open()) {
    string line;
    getline(file, line);

    Found = Found.parse(line);
  } else {
    // Fail on purpose.
    ASSERT_TRUE(true == false);
  }

  ASSERT_EQ(Transaction.dump(), Found.dump());
}
