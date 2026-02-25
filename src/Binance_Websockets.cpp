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
#include <iostream>
#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXWebSocket.h>
#include <ixwebsocket/IXWebSocketServer.h>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>

//  Quality of life statements. All are unneccessary, strictly speaking.
using json = nlohmann::json;
using MessageType = ix::WebSocketMessageType;
using WebSocket = ix::WebSocket;
using WebSocketServer = ix::WebSocketServer;
using MessagePtr = ix::WebSocketMessagePtr;
using namespace std;

int main(int argc, char *argv[]) {
  // Required for Windows.
  ix::initNetSystem();

  // json object to send to Binance to subscribe to the streams.
  json SubscribeJSON;
  SubscribeJSON["method"] = "avgPrice";
  SubscribeJSON["params"]["symbol"] = "BTCUSDT";
  SubscribeJSON["id"] = 1;
  SubscribeJSON["returnRateLimits"] = false;

  cout << SubscribeJSON.dump(2) << endl;

  // Create a WebSocket for getting info from Binance.
  WebSocket BinanceStream;
  BinanceStream.setUrl("wss://ws-api.binance.us:9443/ws-api/v3");

  // Create a WebSocketServer to connect with frontend.
  WebSocketServer Server(8080, "127.0.0.1");

  // Set behavior on message receipt from Binance.
  BinanceStream.setOnMessageCallback(
      [&BinanceStream, &SubscribeJSON, &Server](const MessagePtr &msg) {
        if (msg->type == MessageType::Message) {
          json Received = json::parse(msg->str);

          cout << "Received:\n" << Received.dump(4) << "\n" << endl;

          // Discard what the front will not need.
          json Shortened;
          Shortened["Symbol"] = Received["s"];
          Shortened["Price"] = Received["w"];
          string Sent = Shortened.dump(4);

          // Print sent strings to console.
          cout << "Sent to front:\n" << Sent << "\n" << endl;

          for (auto &&client : Server.getClients()) {
            client->send(Sent);
          }

        } else if (msg->type == MessageType::Open) {
          cout << "Socket is open. Sending:\n" << SubscribeJSON.dump(4) << endl;
          BinanceStream.send(SubscribeJSON.dump(4));
        } else if (msg->type == MessageType::Close) {
          cout << "Socket is closed." << endl;
          BinanceStream.stop();
        }
      });

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

  Server.listenAndStart();
  cout << "Server is up." << endl;
  BinanceStream.run();
}
