#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

/* class Database_Handler
 *
 * Receives frontend transaction information through Binance_Websockets.cpp. Information
 * read in from the Websocket gets saved to database.csv. When called, the Websocket relays
 * data from the database so that it can be sent to the frontend.
 */
class Database_Handler {
public:
    /* inline void recordTransaction()
     *
     * Leverages ios append mode to write the incoming transaction
     * to the end of the CSV database.
     */
    inline void recordTransaction(const string& transactionData) {
        ofstream file("../holdings.csv", ios::app);
        if (file.is_open()) {
            file << transactionData << endl;
        }
    }

    /* inline void sendHoldingsData()
    *
    * Reads each line of database.csv, packs them into a JSON object, and sends them through the
    * Websocket. Final line has "last": true to tell the other component that the file is done.
    */
    inline void sendHoldingsData(ix::WebSocket& webSocket) {
        ifstream file("../database.csv");
        string line;
        vector<json> userHoldings;

        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string type, coin, price, qty, time;

            getline(ss, type, ',');
            getline(ss, coin, ',');
            getline(ss, price, ',');
            getline(ss, qty, ',');
            getline(ss, time, ',');

            json entry;
            entry["coin"] = coin;
            entry["quantity"] = atof(qty.c_str());
            entry["price"] = atof(price.c_str());
            entry["time"] = time;
            entry["last"] = false;
            userHoldings.push_back(entry);
        }

        if (userHoldings.empty()) {
            json empty;
            empty["last"] = true;
            webSocket.send(empty.dump());
            return;
        }

        for (size_t i = 0; i < userHoldings.size(); ++i) {
            if (i == userHoldings.size() - 1) userHoldings[i]["last"] = true;
            webSocket.send(userHoldings[i].dump());
        }
    }
};
