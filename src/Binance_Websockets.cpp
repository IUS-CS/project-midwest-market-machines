/* Binance Connection
 * Backend service for Midwest Market Machines
 * The Simple Trade App
 *
 * Hunter William Poole
 * 2-24-2026
 */

#include "ixwebsocket/IXConnectionState.h"
#include "ixwebsocket/IXWebSocketMessage.h"
#include "ixwebsocket/IXWebSocketMessageType.h"
#include <chrono>
#include <cstddef>
#include <iostream>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketServer.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <random>
#include <string>
#include <thread>

//  Quality of life statements. All are unneccessary, strictly speaking.
using json = nlohmann::json;
using MessageType = ix::WebSocketMessageType;
using WebSocket = ix::WebSocket;
using WebSocketServer = ix::WebSocketServer;
using MessagePtr = ix::WebSocketMessagePtr;
using namespace std;

WebSocketServer Server(8080, "127.0.0.1");
vector<unique_ptr<WebSocket>> SocketsVector;

void CreateServer() {
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
        if (msg->type == MessageType::Close) {
          Server.stop();
        }
      });

  auto res = Server.listen();
  if (res.first) {
    Server.start();
  }
}

void ConnectToWebSocket(const string &coin) {
  auto Socket = make_unique<WebSocket>();
  Socket->setUrl("wss://ws-api.binance.us:9443/ws-api/v3");
  int ID = rand();

  Socket->setOnMessageCallback(
      [S = Socket.get(), Coin = coin, ID = ID](const MessagePtr &msg) {
        json SubscribeJSON;
        // json SubscribeJSON = {
        //     {"id", ID}, {"method", "avgPrice"}, {"params", {"symbol",
        //     Coin}}};
        SubscribeJSON["id"] = ID;
        SubscribeJSON["method"] = "avgPrice";
        SubscribeJSON["params"]["symbol"] = Coin;

        cout << "DUMP SUBSCRIBEJSON\n" << SubscribeJSON.dump(2) << endl;
        if (msg->type == MessageType::Message) {
          json Received = json::parse(msg->str);

          cout << "Received:\n" << Received.dump(2) << endl;

          json Shortened;
          Shortened["id"] = Received["id"];
          Shortened["coin"] = Coin;
          Shortened["price"] = Received["result"]["price"];
          string OutBound = Shortened.dump(2);

          cout << "Sent to client:\n" << OutBound << endl;

          for (auto &&client : Server.getClients()) {
            client->send(OutBound);
            cout << "Client: " << client << endl;
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
  SocketsVector.push_back(move(Socket));
}

int main(int argc, char *argv[]) {
  // Required for Windows.
  ix::initNetSystem();

  string coins[] = {"BTCUSDT", "ETHUSDT", "ADAUSDT",
                    "XRPUSDT", "DOTUSDT", "UNIUSDT"};

  CreateServer();
  // for (const string &coin : coins) {
  //   ConnectToWebSocket(coin);
  // }

  ConnectToWebSocket(coins[0]);

  while (true) {
    this_thread::sleep_for(chrono::seconds(1));
  }
}
