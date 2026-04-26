import { useState } from 'react'
import './App.css'
import Navbar from './components/navBar'
import Watchlist from './components/Watchlist'
import useCryptoSocket from './useCryptoSocket'
import ShowPrice from './components/ShowPrice'
import CandlestickChart from './components/CandlestickChart'
import ChatBox from './components/ChatBox'
/*
  Main component, coordinates other files by connecting 
  1. (currentCoin) state which defaults to BTCUSDT->
  2. Watchlist component when a user clicks on a coin-> 
  3. updates (currentCoin) state-> WebSocket function triggers->
  4. the WebSocket, returning the coin's price and other necessary data->
  5a. passing that price to the ShowPrice component to update the display.
  5b. and the rest of the data, such as start time and KlineFinished, is sent to the CandlestickChart component.
*/
function App() {
/* (1) Initialization: Sets the starting coin and establishes the socket connection */
  const [currentCoin, setCurrentCoin] = useState('BTCUSDT');
  /* (4) Data Channel: WebSocket hook listens for changes to currentCoin and returns price */
  const { price, latestCandle, trade} = useCryptoSocket(currentCoin);

  return (
    <>
      <Navbar />

      <div className="App_header">
        <h1>Simple Trade</h1>
        <p>Your one-stop shop for all things crypto!</p>
      </div>
        <div>
          <button onClick={() => trade('buy', 1.0)}>TEST BUY</button>
          <button onClick={() => trade('sell', 1.0)}>TEST SELL</button>
        </div>
      <div className="main_layout">
        {/* (2) User Input: Watchlist provides the interface for selecting a new coin */}
        {/* (3) State Update: onCoin triggers setCurrentCoin, restarting the cycle at step 1 */}
        <Watchlist onCoin={(c) => setCurrentCoin(c)} currentCoin={currentCoin} />
        <div className="flex_row">
          {/* (5a) Rendered Output: ShowPrice receives the final price and renders it to the screen */}
          <ShowPrice price={price} coin={currentCoin} />
          {/* (5b Rendered Output: CandlestickChart receives the data necessary to create and update the candles)*/}
          <CandlestickChart coin={currentCoin} latestCandle={latestCandle} />
        </div>
      </div>
      
      {/* AI Trading Assistant ChatBox */}
      <ChatBox />
    </>
  )
}

export default App
