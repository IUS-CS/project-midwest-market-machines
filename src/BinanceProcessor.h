/* BinanceProcessor
 * Class header file.
 * The Simple Trade App
 *
 * BinanceProcessor.h
 *
 * This header file creates the BinanceProcessor class.
 * The BinanceProcessor uses two structs, defined outside the class:
 *    1. KlineData, which holds minimal info on the Kline.
 *    2. OutboundJSONStruct, which is the actual struct to be sent out.
 *
 * The BinanceProcessor class exposes one public method:
 *    1. OutboundJSONStruct toSimpleKline(const json &received)
 *        - This method takes a JSON object, and returns the simplified
 *          kline OutboundJSONStruct.
 *          The "meat" of the parsing logic is here.
 */

#pragma once

#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;
using namespace std;

/* These two structs are used to define the shape of the JSON object we want to
 * create from Binance's response.
 *
 * We create struct KlineData to select, well, the KlineData, and struct
 * OutboundJSONStruct to hold the actual JSON we'd like to send out to the
 * frontend.
 *
 * Defining the shape here allows us to change the shape of the JSON object
 * without hunting down the specific lines of code in the client or
 * server. Hence the creation of this header file.
 *
 * Likely to be more useful later than they are now.
 */
struct KlineData {
  string Open;
  string Close;
  string High;
  string Low;
  bool KlineFinished;
};

struct OutboundJSONStruct {
  uint64_t EventTime;
  string Coin;
  KlineData Kline;
};

/* These lines tell nlohmann/json to define KlineData and OutboundJSONStruct as
 *  types.
 *
 *  Should be <type_you_want>, <member_1>, <member_2>, ..., <member_n>
 */
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(KlineData, Open, Close, High, Low);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(OutboundJSONStruct, EventTime, Coin, Kline);

/* class BinanceProcessor exists to define the methods needed to parse Binance's
 * return JSONs into easy-to-read JSONs for the frontend.
 *
 * Largely, the purpose of this class is to abstract away the JSON parsing from
 * any client websocket implementation.
 */
class BinanceProcessor {
public:
  /* toSimpleKline()
   *
   * This function takes a pointer to the response of a Binance WebSocket Kline
   * stream, simplifies it into the basic Kline facts, and then returns a
   * complete OutboundJSONStruct.
   */
  static OutboundJSONStruct toSimpleKline(const json &received) {
    OutboundJSONStruct outbound;
    outbound.EventTime = received["E"];
    outbound.Coin = received["s"];
    outbound.Kline.Open = received["k"]["o"];
    outbound.Kline.Close = received["k"]["c"];
    outbound.Kline.High = received["k"]["h"];
    outbound.Kline.Low = received["k"]["l"];
    outbound.Kline.KlineFinished = received["k"]["x"];
    return outbound;
  }
};
