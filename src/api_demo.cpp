/* Binance API Demo
 * Prototype for Midwest Market Machines
 *
 * Hunter W Poole
 * 2-07-26
 *
 * --------------------------------------------------------------------
 * These #include statements may throw IDE errors depending on whether
 * or not your IDE is aware of the fact that you are using CMake.
 *
 * There are many plugins for CMake if you are using a modern IDE.
 * For VIM - I believe it's something to do with the LSP.
 *
 * In its current form, this program will compile and run without issue.
 * Feel free to ignore the errors your IDE screams about.
 */
#include <iostream>                  // For cout.
#include <ixwebsocket/IXNetSystem.h> // Required as part of ixwebsocket
#include <ixwebsocket/IXUserAgent.h> // Required as part of ixwebsocket
#include <ixwebsocket/IXWebSocket.h> // Required as part of ixwebsocket
#include <nlohmann/json.hpp>         // For making and parsing JSON objects.

int main() {

  /* We declare and instantiate `demojson` from nlohmann::json to format the
   * request payload. Binance communicates back and forth using JSON. As such,
   * we need the nlohmann::json library to work with JSON, such that we may work
   * with Binance.
   *
   * Binance expects a subscription to be as below, where the `id` is not
   * relevant to Binance. `id` may be any number. It is an identifier for our
   * purposes only. Binance will echo the `id` back to us.
   */
  nlohmann::json demojson = {
      {"id", 1},
      {"method", "SUBSCRIBE"},
      {"params", {"btcusdt@trade"}},
  };

  // Create a WebSocket of name `demoSocket`
  ix::WebSocket demoSocket;

  /* We call `.setUrl()` on the WebSocket to set its Url.
   *
   * There are many WebSocket streams available with Binance. We must use
   * `stream.binance.us`, as `strean.binance.com` is not available in the United
   * States due to legal reasons. If for any reason you see you are getting an
   * HTTP 451 status code returned from binance, its probably because you're
   * trying to connect to `.com` instead of `.us`. Double check that.
   *
   * There are many "types" of these streams, for lack of a better word.
   *   Notice, `@trades` appended to the end - this signifies we want to receive
   *   updates about trades.
   *
   *   Other interesting streams you may wish to try:
   *
   *   <symbol>@aggTrade           - Aggregate Trade streams.
   *   <symbol>@kline_1000         - Shows updated candlestick information every
   *                                    1000ms. 1000 is changeable.
   *   <symbol>@miniTicker         - 24hr rolling window mini-ticker statistics.
   *   <symbol>!miniticker@arr     - Shows the mini-ticker for ALL symbols that
   *                                    changed in an array.
   *   <symbol>@ticker             - 24hr rolling stats on this ticker.
   *   <symbol>@avgPrice           - Average price streams.
   *
   *   Check out Binance WebSocket Streams documentation for more information.
   *   https://developers.binance.com/docs/binance-spot-api-docs/web-socket-streams
   */
  demoSocket.setUrl("wss://stream.binance.us:9443/ws/btcusdt@trade");

  /* `.setOnMessageCallback()` is an event listener or event dispatcher for the
   * WebSocket. Since WebSoctets are asynchronous, we don't control when the
   * server decides to send us data, and thus we can't plan when we will read
   * the data from the server. This method is used to define a set of actions
   * that will occur whenever anything is received on the WebSocket.
   *
   * Here, we use a Lambda function (anonymous function) with the following
   * syntax:
   *   `[variables needed / pointers](parameter list){function body};`
   *
   * This formatting is borrowed from the IXWebSocket docs.
   * https://machinezone.github.io/IXWebSocket/
   *
   * You may use `ix::WebSocketMessageType::<type>` to define behavior for a
   * given message type when combined with some filter or conditional statement.
   *
   */
  demoSocket.setOnMessageCallback([&demoSocket, &demojson](
                                      const ix::WebSocketMessagePtr &msg) {
    // Received a Message - get ready to parse JSON.
    if (msg->type == ix::WebSocketMessageType::Message) {
      // Turn the JSON into a string `msg ->str`
      auto data = nlohmann::json::parse(msg->str);

      /* If the data contains "p" then we know we are receiving the `@trade`
       * return JSON.
       *
       * Recall from earlier: we will receive an ECHO from the server to verify
       * receipt of our subscritpion JSON. Unfortunately, it comes as follows:
       *
       * {"result": null, "id": 1} (id is as above.)
       *
       * The null return value of result will crash the program if we try to
       * convert it to a string. You would need to filter that out or put some
       * safeguard around processing that null return value. I have chosen to
       * simply spit it out to the console.
       *
       * Otherwise, assigning `price = data["p];` will cause a crash.
       */
      if (data.contains("p")) {
        // auto data = nlohmann::json::parse(msg->str);
        // std::string price = data["p"];
        // std::string quantity = data["q"];

        // Convert JSON to string and print it. Received: JSON.
        std::cout << "Received: " << msg->str << std::endl;
      } else {
        // This is the defined behavior for when the data *does not* contain "p"
        std::cout << "Return: " << msg->str << std::endl;
      }

      /* This is the defined behavior for if the WebSocketMessageType is `Open`.
       *
       * The Server will send an `Open` message when it verifies the WebSocket
       * is `Open`. From there, you are free to transfer data.
       */
    } else if (msg->type == ix::WebSocketMessageType::Open) {
      std::cout << "Successfully connected to Binance. Please wait for data to "
                   "be sent."
                << std::endl;
      demoSocket.send(demojson.dump());

      // Do *something* to handle and report errors.
    } else if (msg->type == ix::WebSocketMessageType::Error) {
      std::cout << "Error: " << msg->errorInfo.reason << std::endl;
    }
  });

  /* `demosocket.start()` is asynchronous. It will run off and do everything
   * will allowing `main()` to keep working.
   *
   * `demoSocket.run()` is synchronous. It will wait for the connection to die
   * before allowing `main()` to keep working.
   */

  // demoSocket.start();
  demoSocket.run();

  // return 0 for main().
  return 0;
}
