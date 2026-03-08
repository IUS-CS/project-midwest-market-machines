/* Binance Connection
 * Backend service for Midwest Market Machines
 * The Simple Trade App
 *
 * Hunter William Poole
 * 2-24-2026
 *
 * Binance_Websockets.cpp
 * This program spawns one WebSocket server to communicate with the front end,
 * and [x] client WebSockets to communicate with Binance for [x] number of
 * coins, as defined by command line arguments.
 *
 * Default coins are used if no arguments are passed in.
 *
 * The client WebSockets continuously poll Binance for current average price.
 * To do this, they must sent a JSON subscription message detailing the
 * information they want from Binance. Binance's WebSocket API will then send
 * back a JSON object with the requested information. The client WebSocket will
 * parse this information into a JSON object, and collect the data the frontend
 * will need. Then, this JSON object is dumped to the connected client
 * (frontend).
 *
 * To avoid rate limits, the client then sleeps for 4000 milliseconds.
 * This seems to be the optimal time to sleep, since the average price is,
 * afterall, an average over time. It is not likely to change within those 4000
 * milliseconds.
 */

// Minimum necessary includes
#include <chrono>
#include <iostream>
#include <ixwebsocket/IXWebSocketServer.h>
#include <nlohmann/json.hpp>

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
 * This function creates a unique WebSocket using `&coin`.
 * It uses rand() to create an ID to identify each WebSocket by its JSON
 * objects. ID will be used in SubscribeJSON, and Binance will send it back with
 * each reply. Since we know what coin we are creating the socket for, and we
 * have associated a unique ID with that, we are able to identify what coin
 * Binance is sending information about.
 *
 * For the message callback behavior:
 * The currently used Binance API is a "send-on-request" API. So, we have to
 * poll it continuously. As such, we send a message on a delay after we are done
 * processing the last message. We wait 4000 milliseconds to avoid rate limits.
 *
 * Steps, in order:
 * 1. Create a unique WebSocket pointer.
 * 2. Set its URL.
 * 3. Create a unique ID to identify the socket.
 * 4. Build the SubscribeJSON.
 * 5. Send it on message type open.
 * 6. On message receipt, parse the JSON.
 *    6a. Build a shorter JSON of only the pieces we care for at the moment.
 *    6b. Break it into a string.
 *    6c. Send it to each client connected to the server.
 * 7. Print JSON objects sent and received (if DEBUG)
 * 8. Sleep for 4000 milliseconds.
 * 9. Send the SubscribeJSON.
 * 10. On message close, close the WebSocket.
 *
 * TODO: Consider if there is a smarter way to wait. If so, implement it.
 * Current method is probably an anti-pattern.
 */
void ConnectToWebSocket(const string &coin) {
  auto Socket = make_unique<WebSocket>();
  Socket->setUrl("wss://ws-api.binance.us:9443/ws-api/v3");
  int ID = rand();

  json SubscribeJSON;
  SubscribeJSON["id"] = ID;
  SubscribeJSON["method"] = "avgPrice";
  SubscribeJSON["params"]["symbol"] = coin;

  Socket->setOnMessageCallback([S = Socket.get(), SubscribeJSON = SubscribeJSON,
                                Coin = coin](const MessagePtr &msg) {
    if (msg->type == MessageType::Message) {
      json Received = json::parse(msg->str);

      json Shortened;
      Shortened["id"] = Received["id"];
      Shortened["coin"] = Coin;
      Shortened["price"] = Received["result"]["price"];
      string Outbound = Shortened.dump(0);

      for (auto &&client : Server.getClients()) {
        client->send(Outbound);
      }

      if (DEBUG) {
        PrintLocker.lock();
        cout << "SubscribeJSON:\n" << SubscribeJSON.dump(2) << "\n" << endl;
        cout << "Received:\n" << Received.dump(2) << "\n" << endl;
        cout << "Sent to client:\n" << Outbound << "\n" << endl;
        cout << "----------------------------------------" << endl;
        PrintLocker.unlock();
      }
      this_thread::sleep_for(chrono::milliseconds(4000));
      S->send(SubscribeJSON.dump(2));
    } else if (msg->type == MessageType::Open) {
      S->send(SubscribeJSON.dump(2));
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
  // Also known as the <ticker>
  vector<string> coins;

  // If no arguments are supplied, use the default coins.
  if (argc < 2) {
    coins = {"BTCUSDT", "ETHUSDT", "ADAUSDT", "XRPUSDT", "DOTUSDT", "UNIUSDT"};
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
