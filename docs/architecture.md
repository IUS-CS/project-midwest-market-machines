## Introduction

Our project consists of a React-based frontend that handles the User Interface which is connected to a C++ backend. Currently the frontend is built by using terminal commands to pull and install node js packages necessary to the project. However, the backend, because it is built in C++, requires a more comprehensive approach. To build the backend, an OS aware file pulls necessary libraries from their repositories, bootstraps, builds, and links the files. The commands can be automated using .bat, .ps1, .sh, scripts, however due to the changing build environment we have not implemented a 1 click install system yet.

*An activity diagram that shows the logic of the app when building with an OS aware file*
![Diagram](https://github.com/user-attachments/assets/7f0cf3d2-7c0b-4351-8341-c45b454366d2)


## Project Overview

Once the program is built, the flow of data starts from Binance_Websockets.cpp. 
- It requests the necessary information every few seconds from Binance to build the trend of the chart.
- This data is pulled from the backend to the frontend, to useCryptoSocket.jsx.
- useCryptoSocket.jsx pulls which coin to display from Watchlist.jsx and constructs the information needed to make the "candles" for the chart, a way of displaying how much the price of a coin has fluctuated over a given time period, currently set at 1 minute.
- CandlestickChart.jsx pulls that information and uses it to make and update the candles in the chart that it creates at startup.
- If a new coin is selected from the Watchlist component, the chart's data is wiped and new candles begin to form with the new coin's datastream. navBar.jsx and ShowPrice.jsx are currently discrete components, but in the future navBar will link to other pages on the app.
- All of these components, useCryptoSocket, Watchlist, CandlestickChart, navBar, and ShowPrice are called on by App.jsx to build the main page.
  main.jsx is the entry point of the React application, and pulls everything from App.jsx, rendering on launch.  

*This diagram shows the app’s general data flow*
![Diagram](https://github.com/user-attachments/assets/fc51c589-d555-496b-a289-f373603c18da)


## Connectivity

Connections between components are handled via the state coordinator App.jsx. Each component has a narrowly defined purpose, making the [props](https://www.w3schools.com/react/react_props.asp) passed between components are easier to understand and test.  
1. When initialized, the App.jsx’s currentCoin state is set to BTCUSDT, and websocket connection is established. 
2. The Watchlist component allows users to click on the component they wish to track at a given time. If for example ETH were clicked, the update triggers the useCryptoSocket to read the ETHUSDT part of the JSON data by changing currentCoin. 
3. The WebSocket hook in useCryptoSocket listens for changes and returns the price, along with other data necessary to build the CandlestickChart. 
4. The currentCoin filter causes the Showprice component and CandlestickChart to update using ETH data, showing the current price from the most recent Binance JSON in ShowPrice, and building a candle that visualizes the price swings using information such as start time and KlineFinished. 

*A sequence diagram that shows how the components handle a typical chain of inputs*
![Diagram](https://github.com/user-attachments/assets/65e9d9b0-cf5e-412c-a26d-d896def89cba)

*The App.jsx pulls from all other frontend components to build the layout of the page and sends it off to main.jsx to be rendered in the DOM*
![Diagram](https://github.com/user-attachments/assets/f0a71519-d681-4be5-991a-a80fe50a3158)

## Frontend Data Handler

This process is enabled by the useCryptoSocket node, which connects to the backend’s Binance_Websockets and builds a Kline format to display a K-Line or Candlestick graph. The socket URL is defined at local ip 127.0.01 with port 8080, along with other necessary variables candleMapRef, lastCloseRef, and prevCoinRef. A function runs continuously until successful that attempts to connect to the backend and sends logs to the developer console. A useEffect function checks if the previous coin is mapping to the correct coin, which destroys the previous chart by mapping to a new candleMapRef and lastCloseRef. A function then checks for discrepancies in the JSON to ensure the coin is matching the format and selected coin, builds the data into a returnable prop, and passes it to App.jsx to be sorted.

*This activity diagram follows the logic of the app when receiving data from the web socket*
![Diagram](https://github.com/user-attachments/assets/cbf5dd75-ad1e-4e04-a0da-422b8bf881c0)


## Database Schema for Binance Data, formatted in JSON for useCryptoSocket 

*The database schema for Binance data and useCryptoSocket can be seen in this diagram*
![Diagram](https://github.com/user-attachments/assets/302104bc-99ce-4aae-b5bb-dd2b1799da83)


## Binance_Websockets complications

The result is a functional but overly complex role for the Binance_Websockets backend; the breadth of problems Binance_Websockets is responsible for has made testing difficult. Binance_Websockets starts a server and responds to a call from the frontend, setting up the WebSocketServer to check if the server is open, and prints information about the connection in the terminal if debug is checked. The behavior is determined when the server opens, currently set to Binance’s Candlestick Data Stream with a 1 minute interval, extra information is culled, and a cleaned JSON is sent to the frontend. A Websocket pointer is created, the URL is set using &coin, when message is received, JSON is parsed and cleaned, the JSON is broken into a string and then sent to the client connected. JSONs are optionally printed to the console if debug is checked, and if a close message is received, the websocket is terminated.  

Main then takes information about the coin selection and builds a list based on data from the frontend. All information about these coins is then sent to the frontend as a ticker.

*This component diagram details how the Binance server interacts with the app’s web socket*
![Diagram](https://github.com/user-attachments/assets/cf6fbe7e-b0f3-446d-b6c0-82d711dda509)


## Separation of Concerns 

To address this complexity, two classes have been created that separate the JSON parsing logic and the Websocket client logic into selectively testable classes. Furthermore, the increase in separation of concerns for these objects allows for the creation of ExchangeClients that handle different hosts than Binance. 

*A class diagram that shows the fields and functions of the ExchangeClient and BinanceProcessor*
![Diagram](https://github.com/user-attachments/assets/41f8d4e2-1b30-4ebd-8407-cd157d521c73)


## Resulting Test structure 

This approach not only allows the monolithic test file, SmokeTests.cpp, planned for placeholder tests in the backend to be parted out and focused into googletests and ctest modules specific to backend components, but its strategy can be mirrored in the frontend. 

Previously, a monolithic test file was used to implement placeholder tests for the state coordinator, App.test.jsx, with the vague purpose of frontend testing. However, this sprint we have created testing modules to target the behavior of each component created, simplifying the process of planning unit tests.  

*A diagram showing the frontend’s structure converges to the state coordinator, and the discrete components and their testing modules*
![Diagram](https://github.com/user-attachments/assets/25c97141-c738-4975-8379-0a5e0bdb7eca)

*This component diagram shows our approach to testing for the app*
![Diagram](https://github.com/user-attachments/assets/bbafb708-db97-49c9-a73d-b0f50c1b0489)


## Conclusion, Method, Insights

This restructuring has the benefits of simplifying our testing approach, as well as pushing the codebase toward a more coherent and extensible project architecture. Mapping the process for software architecture using written notes and Clip Studio Paint proved too expensive, leading to the diagrams in Obsidian. Using Obsidian's Excalidraw plugin is a step up from drawing diagrams by hand, as it allows you to pick up lines and reconnect flow between diagrams. However manually connecting diagrams with a mouse does not scale well, and despite Obsidian Excalidraw's lower learning curve than PlantUML ; the rate at which PlantUML speeds up the process of creation, along with the ability to refactor diagrams as a codebase, makes PlantUML a superior tool for Agile development.
