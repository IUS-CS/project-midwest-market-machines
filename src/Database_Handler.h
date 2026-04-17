#ifndef DATABASE_HANDLER_H
#define DATABASE_HANDLER_H

#include <fstream>
#include <sstream>
#include <vector>
#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>

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
    inline void recordTransaction(const std::string& transactionData) {
        std::ofstream file("../database.csv", std::ios::app);
        if (file.is_open()) {
            file << transactionData << std::endl;
        }
    }

    /* inline void sendHoldingsData()
    * 
    * Reads each line of database.csv, packs them into a JSON object, and sends them through the
    * Websocket. Final line has "last": true to tell the other component that the file is done.
    */
    inline void sendHoldingsData(ix::WebSocket& webSocket) {
        std::ifstream file("../database.csv");
        std::string line;
        std::vector<nlohmann::json> userHoldings;

        while (std::getline(file, line)) {
            if (line.empty()) continue;
            std::stringstream ss(line);
            std::string type, coin, price, qty, time;

            std::getline(ss, type, ',');
            std::getline(ss, coin, ',');
            std::getline(ss, price, ',');
            std::getline(ss, qty, ',');
            std::getline(ss, time, ',');

            nlohmann::json entry;
            entry["coin"] = coin;
            entry["quantity"] = std::atof(qty.c_str());
            entry["price"] = std::atof(price.c_str());
            entry["time"] = time;
            entry["last"] = false; 
            userHoldings.push_back(entry);
        }

        if (userHoldings.empty()) {
            nlohmann::json empty;
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

#endif
