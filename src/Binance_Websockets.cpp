/* Binance Connection
 * Backend service for Midwest Market Machines
 * The Simple Trade App
 *
 * Hunter William Poole
 * 03-08-2026
 *
 * Binance_Websockets.cpp
 * This program spawns one WebSocket server to communicate with the front end,
 * and [x] WebSocket clients to communicate with Binance for [x] number of
 * coins, as defined by command line arguments. Default coins are used if no
 * arguments are passed in.
 *
 * Will subscribe to the Candestick Data Stream from Binance. Then, prunes the
 * response for relevant information to send off to the front end.
 *
 * TODO: Investigate structs for nlohmann/json to auto serialize into JSON w/o
 * manual mapping.
 */

// Minimum necessary includes
#include "webview.h"
#include <chrono>
#include <iostream>
#include <ixwebsocket/IXWebSocketServer.h>
#include <nlohmann/json.hpp>
#include <thread>

//  Quality of life statements. All are unneccessary, strictly speaking.
using json = nlohmann::json;
using MessageType = ix::WebSocketMessageType;
using WebSocket = ix::WebSocket;
using WebSocketServer = ix::WebSocketServer;
using MessagePtr = ix::WebSocketMessagePtr;
using namespace std;

//-------------------Global Constants-----------------------

// TRUE to print each JSON object and action to console.
// FALSE to not do so.
const bool DEBUG = true;
//----------------------------------------------------------

//-------------------Global Declarations--------------------
WebSocketServer Server(8080, "127.0.0.1");
// Put the WebSockets here so they don't die.
// seg fault if you remove this without implementing something else.
vector<unique_ptr<WebSocket>> SocketsVector;
// Just for printing during DEBUG.
mutex PrintLocker;
//----------------------------------------------------------

/* void StartServer()
 *
 * This function sets the server's behavior on message callback, and
 * starts the server.
 *
 * It will always print details about the remote connection.
 */
void StartServer() {
  // Set behavior on message receipt from frontend.
  Server.setOnClientMessageCallback(
      [](shared_ptr<ix::ConnectionState> connectionState, WebSocket &webSocket,
         const MessagePtr &msg) {
        cout << "Remote ip: " << connectionState->getRemoteIp() << endl;

        if (msg->type == MessageType::Open) {
          cout << "New connection" << endl;
          cout << "Uri: " << msg->openInfo.uri << endl;
          webSocket.send("Connected");
        }
        // Commented out while debugging frontend.
        // if (msg->type == MessageType::Close) {
        // Server.stop();
        //}
      });

  Server.listenAndStart();
}

/* void ConnectToWebSocket(const string &coin)
 *
 * This function creates the client WebSocket connection to Binance's WS Stream
 * API, and determines the behavior on message receipt.
 *
 * Currently, it conencts to Binance's Candlestick Data Stream for a 1 minute
 * interval. It will cull extra information from the received JSON, and send
 * only what is currently necessary to the front.
 *
 * Steps, in order:
 * 1. Create a unique WebSocket pointer.
 * 2. Set its URL from &coin.
 * 3. On message receipt, parse the JSON.
 *    3a. Build a shorter JSON of only the pieces we care for at the moment.
 *    3b. Break it into a string.
 *    3c. Send it to each client connected to the server.
 * 4. Print JSON objects sent and received (if DEBUG)
 * 5. On message close, close the WebSocket.
 */
void ConnectToWebSocket(const string &coin) {
  auto Socket = make_unique<WebSocket>();
  string StreamUrl = "wss://stream.binance.us:9443/ws/" + coin + "@kline_1m";
  cout << "Subscribed to: " << StreamUrl << endl;
  Socket->setUrl(StreamUrl);

  Socket->setOnMessageCallback(
      [S = Socket.get(), Coin = coin](const MessagePtr &msg) {
        if (msg->type == MessageType::Message) {
          json Received = json::parse(msg->str);

          json Shortened;
          Shortened["TimeStamp"] = Received["E"];
          Shortened["Coin"] = Received["s"];
          Shortened["Kline"]["Open"] = Received["k"]["o"];
          Shortened["Kline"]["Close"] = Received["k"]["c"];
          Shortened["Kline"]["High"] = Received["k"]["h"];
          Shortened["Kline"]["Low"] = Received["k"]["l"];
          string Outbound = Shortened.dump(0);

          for (auto &&client : Server.getClients()) {
            client->send(Outbound);
          }

          if (DEBUG) {
            PrintLocker.lock();
            cout << "Received:\n" << Received.dump(2) << "\n" << endl;
            cout << "Sent to client:\n" << Outbound << "\n" << endl;
            cout << "----------------------------------------" << endl;
            PrintLocker.unlock();
          }
        } else if (msg->type == MessageType::Close) {
          S->close();
        }
      });

  Socket->start();
  // Push the socket onto the global vector, so it's not killed.
  // seg faults if you remove this without implementing something else.
  SocketsVector.push_back(move(Socket));
}

/* int main(int argc, char *argv[])
 *
 * main takes the coins you'd like to receive info on as a set of arguments.
 * It will then spawn the server, and spawn a WebSocket for every coin.
 *
 * Currently, it takes no major ownership or control of the threads that are
 * spawned for the WebSockets and the Server.
 *
 * TODO: Find an implement a better thread control mechanism.
 *
 * TODO: Get rid of the while(true) loop with a wait in in.
 * Have to find a better way to keep main alive while threads are running.
 */
int main(int argc, char *argv[]) {
  // Required for Windows.
  ix::initNetSystem();

  // Holds the names of coins.
  // Also known as the <ticker> or <symbol>.
  vector<string> coins;

  // If no arguments are supplied, use the default coins.
  if (argc < 2) {
    coins = {"btcusdt", "ethusdt", "adausdt", "xrpusdt", "dotusdt", "uniusdt"};
  } else {
    for (int i = 0; i < argc; i++) {
      string Argument = argv[i];
      coins.push_back(Argument);
    }
  }

  // Spawn the server. See above.
  StartServer();

  // For each coin, spawn a WebSocket.
  for (const string &coin : coins) {
    ConnectToWebSocket(coin);
  }

  // A genuinely terrible way to make sure main doesn't end while the WebSockets
  // are running.
  while (true) {
    this_thread::sleep_for(chrono::nanoseconds(1));
    // Now I am become Sleep, the destroyer of useful CPU time.
  }

  return 0;
}
