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
#include <random>
#include <string>

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
    filesystem::create_directories("./testData/");

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

/* Class TestDB
 *
 * This is a test fixture class. Its primary purpose is to ensure there is
 * always a Database, Hooks, the proper directory, and that all are torn down on
 * test completion.
 *
 * SetUp() and TearDown() run before and after each test, respectively.
 */
class TestDB : public ::testing::Test {
protected:
  Database_Handler *Database;
  MockDatabaseHooks *Hooks;
  ix::WebSocket *socket;

  void SetUp() override {
    filesystem::create_directories("./testData/");
    Database = new Database_Handler("./testData/");
    Hooks = new MockDatabaseHooks();
  }
  void TearDown() override {
    filesystem::remove_all("./testData/");
    delete Database;
    delete Hooks;
  }
};

//----------------------- GLOBALS ------------------------------------------
int port = 9999;
string host = "127.0.0.1";

/* Record_A_Transaction
 *
 * This test attempts to save a transaction using recordTransaction.
 * Then, we read the test manually from the .csv file.
 *
 * 1. Assemble a JSON of a typical buy transaction.
 * 2. Call the Database to record it.
 * 3. Read from the transactionHistory.csv and parse into a JSON.
 * 4. Assert the input JSON is equal to the found JSON.
 */
TEST_F(TestDB, Record_A_Transaction) {
  json Transaction, Found;
  string line;

  Transaction["type"] = "buy";
  Transaction["coin"] = "BTCUSDT";
  Transaction["price"] = 70808.0;
  Transaction["quantity"] = 1.00;

  Database->recordTransaction(&Transaction);

  ifstream file("./testData/transactionHistory.csv");
  while (getline(file, line)) {
    if (line.empty())
      continue;
    stringstream ss(line);
    string type, coin, price, qty;
    getline(ss, type, ',');
    getline(ss, coin, ',');
    getline(ss, price, ',');
    getline(ss, qty, ',');

    Found["type"] = type;
    Found["coin"] = coin;
    Found["price"] = atof(price.c_str());
    Found["quantity"] = atof(qty.c_str());
  }

  ASSERT_EQ(Transaction.dump(), Found.dump());
}

/* Record_10_Transactions
 *
 * This test functions almost exactly the same as the above test,
 * Record_A_Transaction, except we record 10 transactions, and assert that each
 * transaction read is equal to what was recorded.
 *
 * 1. Generate random of types, coins, prices, quantities...
 * 2. Place those all in their respective vectors.
 * 3. Do this 10 times:
 *    3a. Assemble a JSON from the vectors of semi-random info.
 *    3b. Call the Database to record it.
 *    3c. Read a JSON from the csv.
 *    3d. Assert these JSONs are equal.
 */
TEST_F(TestDB, Record_10_Transactions) {
  json Transaction, Found;
  string line;

  random_device RAND;
  mt19937 Generator(RAND());

  uniform_int_distribution<> CoinFlip(0, 1);
  uniform_int_distribution<> CoinSelector(0, 5);
  uniform_real_distribution<double> DoubleDist(0.0, 100000.0);

  vector<string> TransactionTypes;
  vector<string> Coins;
  vector<double> Prices;
  vector<double> Quantities;

  auto PickACoin = [&]() {
    switch (CoinSelector(Generator)) {
    case 0:
      return "BTCUSDT";
    case 1:
      return "ETHUSDT";
    case 2:
      return "ADAUSDT";
    case 3:
      return "XRPUSDT";
    case 4:
      return "DOTUSDT";
    case 5:
      return "UNIUSDT";
    default:
      return "BTCUSDT";
    }
  };

  for (int i = 0; i < 10; i++) {
    TransactionTypes.push_back(CoinFlip(Generator) == 0 ? "buy" : "sell");
    Coins.push_back(PickACoin());
    Prices.push_back(floor(DoubleDist(Generator)));
    Quantities.push_back(floor(DoubleDist(Generator)));
  }

  for (int i = 0; i < 10; i++) {
    Transaction["type"] = TransactionTypes[i];
    Transaction["coin"] = Coins[i];
    Transaction["price"] = Prices[i];
    Transaction["quantity"] = Quantities[i];

    Database->recordTransaction(&Transaction);

    ifstream file("./testData/transactionHistory.csv");
    while (getline(file, line)) {
      if (line.empty())
        continue;
      stringstream ss(line);
      string type, coin, price, qty;
      getline(ss, type, ',');
      getline(ss, coin, ',');
      getline(ss, price, ',');
      getline(ss, qty, ',');

      Found["type"] = type;
      Found["coin"] = coin;
      Found["price"] = atof(price.c_str());
      Found["quantity"] = atof(qty.c_str());
    }

    file.close();

    ASSERT_EQ(Transaction.dump(), Found.dump());
  }
}
