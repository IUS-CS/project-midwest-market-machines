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
#include "ixwebsocket/IXNetSystem.h"
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
    ix::initNetSystem();
    filesystem::create_directories("./testData/");

    promise<bool> IsReady;
    future<bool> FutureServerReady = IsReady.get_future();

    Server = make_unique<ix::WebSocketServer>(9999, "127.0.0.1");

    Server->setOnClientMessageCallback(
        [](shared_ptr<ix::ConnectionState> connectionState,
           ix::WebSocket &webSocket,
           const ix::WebSocketMessagePtr &msg) -> void {
          Database_Handler db("./testData/");
          Database_Handler Historical("../userData/");

          if (msg->type == ix::WebSocketMessageType::Open) {
            db.sendHoldingsData(webSocket);
            db.sendTransactionHistory(webSocket);

            Historical.sendHistoricalData(webSocket, "btcusdt");
            Historical.sendHistoricalData(webSocket, "ethusdt");
            Historical.sendHistoricalData(webSocket, "adausdt");
            Historical.sendHistoricalData(webSocket, "xrpusdt");
            Historical.sendHistoricalData(webSocket, "dotusdt");
            Historical.sendHistoricalData(webSocket, "uniusdt");
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
  ix::WebSocket *socket;

  void SetUp() override {
    ix::initNetSystem();
    filesystem::create_directories("./testData/");
    Database = new Database_Handler("./testData/");
  }
  void TearDown() override {
    filesystem::remove_all("./testData/");
    delete Database;
  }
};

//----------------------- GLOBALS ------------------------------------------
unique_ptr<ix::WebSocketServer> Database_HandlerTest::Server = nullptr;
thread Database_HandlerTest::ServerThread;

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

/* Holdings_Are_Sent
 *
 * This test uses the Database_HandlerTest fixture.
 * Here, we claim that given any valid holdings in the holdings.csv, the
 * Database WebSocket server will send them on client connection.
 *
 * 1. Put a "basic" holding into "./testData/holdings.csv"
 * 2. Get a promise / future JSON.
 * 3. Make a webSocket client.
 *    3a. Set its url.
 * 4. Set the client's callback behavior.
 *    4a. If we received a message, parse it into a JSON.
 *    4b. If that message is a holding, set our promise to the recieved JSON.
 * 5. Start the client.
 * 6. Assert the future is ready, and wait up to one second for it.
 * 7. Get the result from the future.
 * 8. Expect each member of our result matches what should have been sent.
 *    8a. The data from our "basic" holding.
 *    8b. The dataType of "holding"
 *    8c. The last holding flag is set, since we only inserted one holding.
 * 9. Stop the client
 */
TEST_F(Database_HandlerTest, Holdings_Are_Sent) {
  filesystem::remove_all("./testData/holdings.csv");
  ofstream Holding("./testData/holdings.csv");
  Holding << "BTCUSDT,1.5" << endl;
  Holding.close();

  promise<json> ReceivedHolding;
  future<json> FutureReceivedHolding = ReceivedHolding.get_future();

  ix::WebSocket client;
  client.setUrl("ws://127.0.0.1:9999");
  client.setOnMessageCallback([&](const ix::WebSocketMessagePtr &msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      json Received = json::parse(msg->str);

      if (Received["dataType"] == "holding") {
        try {
          ReceivedHolding.set_value(Received);
        } catch (const future_error &error) {
        }
      }
    }
  });

  client.start();

  ASSERT_EQ(FutureReceivedHolding.wait_for(chrono::seconds(1)),
            future_status::ready);

  json result = FutureReceivedHolding.get();

  EXPECT_EQ(result["coin"], "BTCUSDT");
  EXPECT_EQ(result["quantity"], 1.5);
  EXPECT_EQ(result["dataType"], "holding");
  EXPECT_EQ(result["last"], true);

  client.stop();
  filesystem::remove_all("./testData/holdings.csv");
}

/* Transactions_Are_Sent
 *
 * This test uses the Database_HandlerTest fixture.
 * Here, we claim that when given a valid transaction to send, and a webSocket
 * client connecting to a server calling the sendTransactionHistory() method, we
 * receive a transaction from transactionHistory.csv
 *
 * 1. Put a "basic" transaction into "./testData/transactionHistory.csv"
 * 2. Get a promise / future JSON.
 * 3. Make a webSocket client.
 *    3a. Set its url.
 * 4. Set the client's callback behavior.
 *    4a. If we received a message, parse it into a JSON.
 *    4b. If that message is a transaction, set our promise to the received
 *        JSON.
 * 5. Start the client.
 * 6. Assert the future is ready, and wait up to one second for it.
 * 7. Get the result from the future.
 * 8. Expect that each member of our result matches what should have been sent.
 *    8a. The data from data from our "basic" transaction.
 *    8b. The dataType of "transaction"
 *    8c. The last transaction flag is set, since we only inserted one
 *        transaction.
 * 9. Stop the client.
 */
TEST_F(Database_HandlerTest, Transactions_Are_Sent) {
  filesystem::remove_all("./testData/transactionHistory.csv");
  ofstream Transaction("./testData/transactionHistory.csv");
  Transaction << "buy,BTCUSDT,70808.2,3,1777423915" << endl;
  Transaction.close();

  promise<json> ReceivedTransaction;
  future<json> FutureReceivedTransaction = ReceivedTransaction.get_future();

  ix::WebSocket client;
  client.setUrl("ws://127.0.0.1:9999");
  client.setOnMessageCallback([&](const ix::WebSocketMessagePtr &msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      json Received = json::parse(msg->str);

      if (Received["dataType"] == "transaction") {
        try {
          ReceivedTransaction.set_value(Received);
        } catch (const future_error &error) {
        }
      }
    }
  });

  client.start();

  ASSERT_EQ(FutureReceivedTransaction.wait_for(chrono::seconds(1)),
            future_status::ready);

  json result = FutureReceivedTransaction.get();

  EXPECT_EQ(result["dataType"], "transaction");
  EXPECT_EQ(result["type"], "buy");
  EXPECT_EQ(result["coin"], "BTCUSDT");
  EXPECT_EQ(result["price"], 70808.2);
  EXPECT_EQ(result["quantity"], 3);
  EXPECT_EQ(result["last"], true);

  client.stop();
  filesystem::remove_all("./testData/transactionHistory.csv");
}

/* Ten_Transactions_Are_Sent
 *
 * Uses the Database_HandlerTest fixture. Uses the same setup and general logic
 * from Record_10_Transactions to record 10 random transactions. We then assert
 * that given a webSocket client and a server which calls
 * sendTransactionHistory(), we will get the same 10 random transactions back.
 *
 * 1.  Generate random of types, coins, prices, quantities...
 * 2.  Place those all in their respective vectors.
 * 3.  Do this 10 times:
 *     3a. Assemble a JSON from the vectors of semi-random info.
 *     3b. Call the Database to record it.
 *     3c. Record onto TransactionsVector.
 * 4.  Get a promise / future for a JSON vector.
 * 5.  Make a webSocket client.
 *     5a. Set its url.
 * 6.  Set the client's callback behavior
 *     6a. If we received a message, parse it into a JSON.
 *     6b. If that message is a transaction, push it onto the vector.
 *         a. If it is the last transaction, set our promise to the JSON vector.
 * 7.  Start the client.
 * 8.  Assert the future is ready, and wait up to one second for it.
 * 9.  Get the result from the future.
 * 10. Expect that each member of our result matches what should have been sent.
 * 11. Stop client, clean up pointers.
 */
TEST_F(Database_HandlerTest, Ten_Transactions_Are_Sent) {
  filesystem::remove_all("./testData/transactionHistory.csv");
  Database_Handler *Database = new Database_Handler("./testData/");
  mutex PushLocker;
  json Transaction;
  vector<json> TransactionsVector;

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
    TransactionsVector.push_back(Transaction);
  }

  vector<json> ReceivedJSONs;
  promise<vector<json>> ReceivedPromise;
  future<vector<json>> FutureReceivedPromise = ReceivedPromise.get_future();

  ix::WebSocket client;
  client.setUrl("ws://127.0.0.1:9999");
  client.setOnMessageCallback([&](const ix::WebSocketMessagePtr &msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      json received = json::parse(msg->str);

      if (received["dataType"] == "transaction") {
        lock_guard<mutex> lock(PushLocker);
        ReceivedJSONs.push_back(received);

        if (received["last"] == true || ReceivedJSONs.size() == 10) {
          try {
            ReceivedPromise.set_value(ReceivedJSONs);
          } catch (const future_error &error) {
          }
        }
      }
    }
  });

  client.start();

  ASSERT_EQ(FutureReceivedPromise.wait_for(chrono::seconds(1)),
            future_status::ready);
  vector<json> received = FutureReceivedPromise.get();

  for (int i = 0; i < received.size(); i++) {
    EXPECT_EQ(received[i]["coin"], TransactionsVector[i]["coin"]);
    EXPECT_EQ(received[i]["price"], TransactionsVector[i]["price"]);
    EXPECT_EQ(received[i]["quantity"], TransactionsVector[i]["quantity"]);
    EXPECT_EQ(received[i]["type"], TransactionsVector[i]["type"]);
  }

  client.stop();
  delete Database;
  filesystem::remove_all("./testData/transactionHistory.csv");
}

/* Sends_Historical_Data
 *
 * Uses the Database_HandlerTest fixture.
 * In this test, we expect to receive historical data when given a valid
 * webSocket client, and a server that calls sendHistoricalData(). In this case,
 * we use the provided historical data in the userData directory.
 *
 * 1. Get a promise / future of JSON vectors.
 * 2. Make a webSocket client.
 *    2a. Set its url.
 * 3. Set the client's callback behavior.
 *    3a. If we get a message, parse it into a JSON.
 *    3b. If the message's dataType is "historical", push it onto
 *        ReceivedVector. 3c. If the last flag is set, set the ReceivedPromise
 *        with ReceivedVector.
 * 4. Start the client.
 * 5. Assert the future is ready, and wait up to one second for it.
 * 6. Get the result from the future.
 * 7. For each received JSON:
 *    7a. Expect any two adjacent JSONs are not equal (no duplicate histories).
 *    7b. Expect dataType is not null, is a string, and is "historical"
 *    7c. Expect coin is not null, is a number, and is any of "the coins"
 *    7d. Expect time is not null, is a number, and strictly increments.
 *    7e. Expect price is not null, is a number, and differs from its neighbors.
 * 8. Stop the client.
 */
TEST_F(Database_HandlerTest, Sends_Historical_Data) {
  mutex PushLocker;
  vector<json> ReceivedVector;
  promise<vector<json>> ReceivedPromise;
  future<vector<json>> FutureReceivedPromise = ReceivedPromise.get_future();

  ix::WebSocket client;
  client.setUrl("ws://127.0.0.1:9999");
  client.setOnMessageCallback([&](const ix::WebSocketMessagePtr &msg) {
    if (msg->type == ix::WebSocketMessageType::Message) {
      json received = json::parse(msg->str);

      if (received["dataType"] == "historical") {
        lock_guard<mutex> lock(PushLocker);
        ReceivedVector.push_back(received);

        if (received["last"] == true) {
          try {
            ReceivedPromise.set_value(ReceivedVector);
          } catch (const future_error &error) {
          }
        }
      }
    }
  });

  client.start();

  ASSERT_EQ(FutureReceivedPromise.wait_for(chrono::seconds(1)),
            future_status::ready);

  vector<json> Received = FutureReceivedPromise.get();

  for (int i = 0; i < Received.size() - 1; i++) {
    EXPECT_NE(Received[i], Received[i + 1]);

    EXPECT_FALSE(Received[i]["dataType"].is_null());
    EXPECT_TRUE(Received[i]["dataType"].is_string());
    EXPECT_EQ(Received[i]["dataType"], "historical");

    EXPECT_FALSE(Received[i]["coin"].is_null());
    EXPECT_TRUE(Received[i]["coin"].is_string());
    EXPECT_THAT(Received[i]["coin"],
                testing::AnyOf(testing::Eq("btcusdt"), testing::Eq("ethusdt"),
                               testing::Eq("adausdt"), testing::Eq("xrpusdt"),
                               testing::Eq("dotusdt"), testing::Eq("uniusdt")));

    EXPECT_FALSE(Received[i]["time"].is_null());
    EXPECT_TRUE(Received[i]["time"].is_number());
    EXPECT_NE(Received[i]["time"], Received[i + 1]["time"]);
    EXPECT_TRUE(Received[i]["time"] < Received[i + 1]["time"]);

    EXPECT_FALSE(Received[i]["price"].is_null());
    EXPECT_TRUE(Received[i]["price"].is_number());
    EXPECT_NE(Received[i]["price"], Received[i + 1]["price"]);
  }

  client.stop();
}
