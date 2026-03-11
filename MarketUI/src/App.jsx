import { useState } from 'react'
import './App.css'
import Navbar from './components/navBar'
import Watchlist from './components/Watchlist'
import WebSocket from './WebSocket'
import ShowPrice from './components/ShowPrice'
import defaultWatchlist from './LocalStorage/defaultWatchlist'

/*
  Main component, coordinates other files by connecting 
  1. (currentCoin) state which defaults to BTCUSDT->
  2. Watchlist component when a user clicks on a coin-> 
  3. updates (currentCoin) state-> WebSocket function triggers->
  4. the WebSocket, returning the coin's price->
  5. passing that price to the ShowPrice component to update the display.
*/
function App() {
/* (1) Initialization: Sets the starting coin and establishes the socket connection */
  const [currentCoin, setCurrentCoin] = useState('BTCUSDT');
  const [watchlist, setWatchlist] = useState(defaultWatchlist.coins);
  /* (4) Data Channel: WebSocket hook listens for changes to currentCoin and returns price */
  const { price } = WebSocket(currentCoin);
  // this will be the function to handle adding a coin to the list
  const handleAddCoin = (coin) => {
    // standardize input
    const formattedCoin = coin.toUpperCase().trim();
    if (formattedCoin && !watchlist.includes(formattedCoin)) {
      setWatchlist([...watchlist, formattedCoin]);
      }
  };

// this will be a the function to handle the removal of coins
const handleRemoveCoin = (coin) => {
    setWatchlist(watchlist.filter((item)=> item!==coin));

  };

  return (
    <>
      <Navbar />

      <div className="App_header">
        <h1>Simple Trade</h1>
        <p>Your one-stop shop for all things crypto!</p>
      </div>
      {/* (2) User Input: Watchlist provides the interface for selecting a new coin */}
      {/* (3) State Update: onCoin triggers setCurrentCoin, restarting the cycle at step 1 */}
      <Watchlist coins={watchlist}

        onAddCoin={handleAddCoin}

        onRemoveCoin={handleRemoveCoin}

        onCoin={(c) => setCurrentCoin(c)}

        currentCoin={currentCoin}
      />
      {/* (5) Rendered Output: ShowPrice receives the final price and renders it to the screen */}
      <ShowPrice price={price} coin={currentCoin} />
    </>
  )
}

export default App
