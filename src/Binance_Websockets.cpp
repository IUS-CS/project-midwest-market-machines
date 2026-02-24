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

using json = nlohmann::json;
using MessageType = ix::WebSocketMessageType;
using WebSocket = ix::WebSocket;
using WebSocketServer = ix::WebSocketServer;
using MessagePtr = ix::WebSocketMessagePtr;
using namespace std;

int main(int argc, char *argv[]) {
  // Required for Windows.
  ix::initNetSystem();

  WebSocket GetBTC;
  GetBTC.setUrl("wss://stream.binance.us:9443/ws/btcusdt@avgPrice");

  WebSocket GetETH;
  GetETH.setUrl("wss://stream.binance.us:9443/ws/ethusdt@avgPrice");

  WebSocket GetADA;
  GetADA.setUrl("wss://stream.binance.us:9443/ws/adausdt@avgPrice");

  WebSocket GetXRP;
  GetXRP.setUrl("wss://stream.binance.us:9443/ws/xrpusdt@avgPrice");

  WebSocket GetDOT;
  GetDOT.setUrl("wss://stream.binance.us:9443/ws/dotusdt@avgPrice");

  WebSocket GetUNI;
  GetUNI.setUrl("wss://stream.binance.us:9443/ws/uniusdt@avgPrice");

  // Create a WebSocketServer to connect with frontend.
  WebSocketServer Server(8080, "127.0.0.1");

  // Set behavior on message receipt from frontend.
  Server.setOnClientMessageCallback(
      [&Server](shared_ptr<ix::ConnectionState> connectionState,
                WebSocket &webSocket, const ix::WebSocketMessagePtr &msg) {
        cout << "Remote ip: " << connectionState->getRemoteIp() << endl;

        if (msg->type == MessageType::Open) {
          cout << "New connection" << endl;
          cout << "Uri: " << msg->openInfo.uri << endl;
          webSocket.send("Connected");
        }
        if (msg->type == MessageType::Close) {
        }
      });
}
