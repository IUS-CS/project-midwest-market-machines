#pragma once

#include <fstream>
#include <sstream>
#include <vector>
#include <map>

#include <ixwebsocket/IXWebSocket.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;

/* class Database_Handler
 * Receives frontend transaction information through Binance_Websockets.cpp,
 * Writes transactions to transactionHistory.csv and updates holdings.csv,
 * On connection open, sends current holdings, transaction history, and
 * historical price data to the frontend, with data from each wrapped with tags to allow for 
 * differentiation on the frontend
 */
class Database_Handler {
public:
    /* inline void recordTransaction()
     * On receiving a transaction from the frontend, this function writes to transactionHistory.csv, 
     * then updates holdings.csv by applying the target 
     * "type", (buy/sell) and 
     * "quantity" to the relevant 
     * "coin" using a map function.
     */
    inline void recordTransaction(json* transactionData) {
        // Helper function to convert JSON values  to strings
        auto toStr = [](const json& value) -> std::string {
            if (value.is_string()) return value.get<std::string>();
            return value.dump(); // raw text for non-strings
        };

        string type = toStr((*transactionData)["type"]);
        string coin = toStr((*transactionData)["coin"]);
        double quantity = (*transactionData)["quantity"].get<double>();
    // Transactions:
    // Leverages ios append mode to write the incoming transactionto the end of the CSV database.
        {
            ofstream file("../transactionHistory.csv", ios::app);
            if (file.is_open()) {
                file << type << "," 
                     << coin << ","
                     << toStr((*transactionData)["price"]) << ","
                     << quantity << "," << time(0) << endl;
            } else {
                printf("Database_Handler: error writing to transactionHistory.csv\n"); // Excessive test logging
            }
        }
    /* Holdings:
     * Reads holdings.csv into a map, applies the buy/sell behavior, rewrites the file
     */
        map<string, double> holdings;
        {
            ifstream file("../holdings.csv");
            if (!file.is_open()) {
                printf("Database_Handler: error reading holdings.csv\n"); // Excessive test logging
            } else {
                string line;
                while (getline(file, line)) {
                    if (line.empty()) continue;
                    stringstream ss(line);
                    string mappedCoin, mappedQty;
                    getline(ss, mappedCoin, ',');
                    getline(ss, mappedQty, ',');
                    holdings[mappedCoin] = atof(mappedQty.c_str());
                }
            }
        }

        // Apply negative/positive behavior based on JSON field "type"
        if (type == "buy") {
            holdings[coin] += quantity;
        } else if (type == "sell") {
            holdings[coin] -= quantity;
            //* Boundary case to test coin removal, not permanent, as Bounds checking was supposed to be done in frontend 
            if (holdings[coin] <= 0) holdings.erase(coin); 
        }

        // Rewrite holdings.csv
        {
            ofstream file("../holdings.csv", ios::trunc);
            if (!file.is_open()) {
                printf("Database_Handler: error writing to holdings.csv\n"); // Excessive test logging
            } else {
                for (auto& [mappedCoin, mappedQty] : holdings) {
                    file << mappedCoin << "," << mappedQty << endl;
                }
            }
        }
    }

    /* inline void sendHoldingsData()
     * Reads holdings.csv into a map and sends each coin/quantity pair
     * to the frontend tagged with dataType: "holding".
     * Final entry has last: true.
     */
    inline void sendHoldingsData(ix::WebSocket& webSocket) {
        map<string, double> userHoldings;
        {
            ifstream file("../holdings.csv");
            if (!file.is_open()) {
                printf("Database_Handler: error reading holdings.csv\n"); // Excessive test logging
                json empty;
                empty["dataType"] = "holding";
                empty["last"] = true;
                webSocket.send(empty.dump());
                return;
            }
            string line;
            while (getline(file, line)) {
                if (line.empty()) continue;
                stringstream ss(line);
                string mappedCoin, mappedQty;
                getline(ss, mappedCoin, ',');
                getline(ss, mappedQty, ',');
                userHoldings[mappedCoin] = atof(mappedQty.c_str());
            }
        }

        if (userHoldings.empty()) {
            json empty;
            empty["dataType"] = "holding";
            empty["last"] = true;
            webSocket.send(empty.dump());
            return;
        }

        size_t i = 0;
        for (auto& [coin, qty] : userHoldings) {
            json entry;
            entry["dataType"] = "holding";
            entry["coin"] = coin;
            entry["quantity"] = qty;
            entry["last"] = (i == userHoldings.size() - 1);
            webSocket.send(entry.dump());
            i++;
        }
    }

    /* inline void sendTransactionHistory()
     * Reads transactionHistory.csv and sends each record to the frontend
     * tagged with dataType: "transaction", for order of operations,
     * Final entry has last: true.
     */
    inline void sendTransactionHistory(ix::WebSocket& webSocket) {
        ifstream file("../transactionHistory.csv");
        if (!file.is_open()) {
            printf("Database_Handler: could not open transactionHistory.csv\n"); // Excessive test logging
            json empty;
            empty["dataType"] = "transaction";
            empty["last"] = true;
            webSocket.send(empty.dump());
            return;
        }

        string line;
        vector<json> transactions;

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
            entry["dataType"] = "transaction";
            entry["type"] = type;
            entry["coin"] = coin;
            entry["price"] = atof(price.c_str());
            entry["quantity"] = atof(qty.c_str());
            entry["time"] = time;
            entry["last"] = false;
            transactions.push_back(entry);
        }

        if (transactions.empty()) {
            json empty;
            empty["dataType"] = "transaction";
            empty["last"] = true;
            webSocket.send(empty.dump());
            return;
        }

        for (size_t i = 0; i < transactions.size(); ++i) {
            if (i == transactions.size() - 1) transactions[i]["last"] = true;
            webSocket.send(transactions[i].dump());
        }
    }

    /* inline void sendHistoricalData()
     * 
     * Takes a coin ticker (e.g. "btcusdt") and maps it to its CSV filename
     * (e.g. "btc-usd-max.csv"). Reads timestamp and price, discards
     * market_cap and total_volume. Skips the header row,
     * Wraps JSON entries with dataType: "historical" and sends them to the frontend on connection open,
     * Final entry has last: true to signal completion for frontend
     */
    inline void sendHistoricalData(ix::WebSocket& webSocket, const string& coin) {
        map<string, string> fileMap = {
            {"btcusdt", "btc-usd-max.csv"},
            {"ethusdt", "eth-usd-max.csv"},
            {"adausdt", "ada-usd-max.csv"},
            {"xrpusdt", "xrp-usd-max.csv"},
            {"dotusdt", "dot-usd-max.csv"},
            {"uniusdt", "uni-usd-max.csv"}
        };

        if (fileMap.find(coin) == fileMap.end()) {
            printf("Database_Handler: no file mapping for coin %s\n", coin.c_str()); // Excessive test logging
            return;
        }

        ifstream file("../" + fileMap[coin]);
        if (!file.is_open()) {
            printf("Database_Handler: could not open %s\n", fileMap[coin].c_str()); // Excessive test logging
            json empty;
            empty["dataType"] = "historical";
            empty["coin"] = coin;
            empty["last"] = true;
            webSocket.send(empty.dump());
            return;
        }

        string line;
        vector<json> candles;

        getline(file, line); // Skip CSV column header row

        while (getline(file, line)) {
            if (line.empty()) continue;
            stringstream ss(line);
            string timestamp, price, marketCap, totalVolume;

            getline(ss, timestamp,   ',');
            getline(ss, price,       ',');
            // marketCap and totalVolume are discarded, but we need to read them in to get to the next line
            getline(ss, marketCap,   ','); 
            getline(ss, totalVolume, ','); 

            json entry;
            entry["dataType"] = "historical";
            entry["coin"] = coin;
            entry["time"] = atol(timestamp.c_str());
            entry["price"] = atof(price.c_str());
            entry["last"] = false;
            candles.push_back(entry);
        }

        if (candles.empty()) {
            json empty;
            empty["dataType"] = "historical";
            empty["coin"] = coin;
            empty["last"] = true;
            webSocket.send(empty.dump());
            return;
        }

        for (size_t i = 0; i < candles.size(); ++i) {
            if (i == candles.size() - 1) candles[i]["last"] = true;
            webSocket.send(candles[i].dump());
        }
    }
};
