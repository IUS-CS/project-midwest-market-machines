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

WebSocketServer Server;

void CreateServer() {
  WebSocketServer Server(8080, "127.0.0.1");

  // Set behavior on message receipt from frontend.
  Server.setOnClientMessageCallback(
      [&Server](shared_ptr<ix::ConnectionState> connectionState,
                WebSocket &webSocket, const MessagePtr &msg) {
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
}

void ConnectToWebSocket(const string &coin) {
  WebSocket Socket;
  Socket.setUrl("wss://ws-api.binance.us:9443/ws-api/v3");

  mt19937 mt(time(nullptr));
  long ID = mt();

  json SubscribeJSON = {
      {"id", ID},
      {"method", "avgPrice"},
      {"params", {"symbol", coin}},
  };

  Socket.setOnMessageCallback(
      [&Socket, coin, &SubscribeJSON](const MessagePtr &msg) {
        if (msg->type == MessageType::Message) {
          json Received = json::parse(msg->str);

          json Shortened;
          Shortened["id"] = Received["id"];
          Shortened["price"] = Received["price"];
          string OutBound = Shortened.dump(2);

          for (auto &&client : Server.getClients()) {
            client->send(OutBound);
          }

          this_thread::sleep_for(chrono::milliseconds(2000));
        } else if (msg->type == MessageType::Open) {
          Socket.send(SubscribeJSON.dump(2));
        } else if (msg->type == MessageType::Close) {
          Socket.close();
        }
      });
}

int main(int argc, char *argv[]) {
  // Required for Windows.
  ix::initNetSystem();

  // Create a WebSocketServer to connect with frontend.
  WebSocketServer Server(8080, "127.0.0.1");

  Server.listenAndStart();
  cout << "Server is up." << endl;
  BTC_Stream.run();
  // BTC_Stream.run();
}
