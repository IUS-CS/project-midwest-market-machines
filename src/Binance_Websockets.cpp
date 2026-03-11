/* Binance Connection
 * Backend service for Midwest Market Machines
 * The Simple Trade App
 *
 * Hunter William Poole
 * 03-10-2026
 *
 * Binance_Websockets.cpp
 *
 * This program spawns one WebSocket server to communicate with the front end,
 * and [x] WebSocket clients to communicate with Binance for [x] number of
 * coin. Default coins are used as a stand-in.
 *
 * Will subscribe to the Candestick Data Stream from Binance, and passes the
 * response back to the client.
 */

// Minimum necessary includes
#include "ExchangeClient.h"
#include "webview/webview.h"
#include <filesystem>
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
//----------------------------------------------------------

/* void StartServer()
 *
 * This function sets the server's behavior on message callback, and
 * starts the server.
 *
 * Message callback behavior needs to be set to avoid a warning from
 * ixwebsockets, but we don't need the server to do anything yet, so that method
 * just has an empty body.
 *
 * As in - "No behavior please."
 *
 * Important: StartServer() uses Server.listenAndStart(), which is a BLOCKING
 * call. If you are to later try and join a server thread, you must first call
 * Server.stop().
 */
void StartServer() {
  Server.setOnClientMessageCallback(
      [](shared_ptr<ix::ConnectionState> connectionState, WebSocket &webSocket,
         const MessagePtr &msg) {});

  // Blocking call - need to explicitly call Server.stop() later.
  Server.listenAndStart();
}

/* void StartWebview()
 *
 * Takes one argument, filesystem::path FrontendPath. This is the file path that
 * points to index.html. It is a build artifact excepted to be produced from:
 * `npm run build`.
 *
 * 1. Define window `Webview Window`
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
void StartWebview(filesystem::path FrontendPath) {
  WebView Window(true, nullptr);
  Window.set_title("Simple Trade");
  Window.set_size(1200, 800, WEBVIEW_HINT_NONE);
  Window.navigate("file://" + FrontendPath.generic_string());
  Window.run();
}

/* int main()
 *
 * main creates a vector of coins, using the default coins.
 * It will then spawn background threads for the server and all coins.
 *
 * main() then launches the webview window with a call to
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

  // Spawn the server in a thread.
  thread ServerThread(StartServer);

  // Keep-alive vectors. Save *outside of* the loop.
  vector<unique_ptr<ExchangeClient>> ExchangeClientPointersVector;
  vector<thread> clientThreadsVector;

  // For each coin make a unique client, setDEBUG, setCallback and put on
  // vector.
  for (const string &coin : coins) {
    auto client = make_unique<ExchangeClient>();
    client->setDEBUG(true);
    client->SetCallback([](const string &msg) {
      for (auto &&client : Server.getClients()) {
        client->send(msg);
      }
    });

    string uri = "wss://stream.binance.us:9443/ws/" + coin + "@kline_1m";

    clientThreadsVector.emplace_back(
        [client = client.get(), uri]() { client->Connect(uri); });

    ExchangeClientPointersVector.push_back(move(client));
  }

  // Build relative filepath to the frontend's index.html build artifact.
  // Presumes you ran this file from `build/` in the project's root directory.
  filesystem::path FrontendPath =
      filesystem::current_path() / ".." / "MarketUI" / "dist" / "index.html";

  // Abstracts webview details out of main.
  // In this way, we could launch multiple webviews if desired.
  StartWebview(FrontendPath);

  /* Server.listenAndStart() is a blocking call.
   * We need to explicitly call for the server's death.
   * Then, join the threads and exit.
   */
  Server.stop();
  ServerThread.join();

  for (auto &client : clientThreadsVector) {
    client.join();
  }

  return 0;
}
