/* Binance Connection
 * Backend service for Midwest Market Machines
 * The Simple Trade App
 *
 * Hunter William Poole
 * 03-08-2026
 *
 * Binance_Websockets.cpp
 *
 * This program spawns one WebSocket server to communicate with the front end,
 * and [x] WebSocket clients to communicate with Binance for [x] number of
 * coins, as defined by command line arguments. Default coins are used if no
 * arguments are passed in.
 *
 * Will subscribe to the Candestick Data Stream from Binance. Then, prunes the
 * response for relevant information to send off to the front end.
 *
 * The server
 */

// Minimum necessary includes
#include "BinanceProcessor.h"
#include "ExchangeClient.h"
#include "webview/webview.h"
#include <filesystem>
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
using WebView = webview::webview;
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
 * Message callback behavior needs to be set to avoid a warning from
 * ixwebsockets, but we don't need the server to do anything yet, so that method
 * just has an empty body.
 */
void StartServer() {
  Server.setOnClientMessageCallback(
      [](shared_ptr<ix::ConnectionState> connectionState, WebSocket &webSocket,
         const MessagePtr &msg) {});

  Server.listenAndStart();
}

/* void ConnectToWebSocket(const string &coin)
 *
 * This function creates the client WebSocket connection to Binance's WS Stream
 * API, and determines the behavior on message receipt.
 *
 * Currently, it connects to Binance's Candlestick Data Stream for a 1 minute
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

/*
void ConnectToWebSocket(const string &coin) {
  auto Socket = make_unique<WebSocket>();
  string StreamUrl = "wss://stream.binance.us:9443/ws/" + coin + "@kline_1m";
  cout << "Subscribed to: " << StreamUrl << endl;
  Socket->setUrl(StreamUrl);

  Socket->setOnMessageCallback(
      [S = Socket.get(), Coin = coin](const MessagePtr &msg) {
        if (msg->type == MessageType::Message) {
          json Received = json::parse(msg->str);

          json Shortened = BinanceProcessor::toSimpleKline(Received);
          string OutboundString = Shortened.dump(0);

          for (auto &&client : Server.getClients()) {
            client->send(OutboundString);
          }

          if (DEBUG) {
            PrintLocker.lock();
            cout << "Received:\n" << Received.dump(2) << "\n" << endl;
            cout << "Sent to client:\n" << OutboundString << "\n" << endl;
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

*/

/* int main()
 *
 * main creates a vector of coins, using the default coins.
 * It will then spawn the server, and spawn a WebSocket for every coin in
 * background threads.
 *
 * Afterwards, it launches the WebView window - sourcing the contents from the
 * frontend's `dist` folder. Vite needs to output a single file `index.html` for
 * this to work. So - the frontend is static in this implementation. e.g. no hot
 * reloading. The frontend must be re-built after every change, and this
 * application relaunched.
 *
 * TODO: Accept some flag or argument to enable hot reloading for development
 * quality of life. Will point to vite's dev server rather than the built
 * index.html.
 */
int main() {
  // Required for Windows.
  ix::initNetSystem();

  // Holds the names of coins.
  // Also known as the <ticker> or <symbol>.
  vector<string> coins = {"btcusdt", "ethusdt", "adausdt",
                          "xrpusdt", "dotusdt", "uniusdt"};

  // Build relative filepath to the frontend's index.html build artifact.
  // Presumes you ran this file from `build/` in the project's root directory.
  filesystem::path FrontendPath =
      filesystem::current_path() / ".." / "MarketUI" / "dist" / "index.html";

  // Spawn the server in a thread.
  thread ServerThread(StartServer);

  // For each coin, spawn a client thread.

  /*
  thread ClientThread([&coins]() {
    for (const string &coin : coins) {
      ConnectToWebSocket(coin);
    }
  });
  */

  ExchangeClient testClient;
  testClient.SetCallback([](const string &msg) {
    for (auto &&client : Server.getClients()) {
      client->send(msg);
      cout << msg << endl;
    }
  });

  testClient.Connect("wss://stream.binance.us:9443/ws/btcusdt@kline_1m");

  /* 1. Define window `Webview Window`
   *    1a. debug = true (allows F12 developer menu).
   *    1b. window = nullptr (No window exists yet - make a new one).
   * 2. Set its title.
   * 3. Set its default size.
   *    3a. width = 1200.
   *    3b. height = 800.
   *    3c. hints = ...NONE(No resizing restrictions).
   *        3c-1. Can do MIN, MAX, FIXED if desired.
   * 4. Set where the window goes.
   *    4a. Currently goes to `index.html` frontend build artifact.
   * 5. Then, run the window.
   */
  WebView Window(true, nullptr);
  Window.set_title("Simple Trade");
  Window.set_size(1200, 800, WEBVIEW_HINT_NONE);
  Window.navigate("file://" + FrontendPath.generic_string());
  Window.run();

  /* Server.listenAndStart() is a blocking call.
   * We need to explicitly call for the server's death.
   * Then, join the threads and exit.
   */
  Server.stop();
  ServerThread.join();
  // ClientThread.join();

  return 0;
}
