import { useState } from 'react'
import './App.css'
import Navbar from './components/navBar'
import Watchlist from './components/Watchlist'
import WebSocket from './WebSocket'
import ShowPrice from './components/ShowPrice'
import defaultWatchlist from './LocalStorage/defaultWatchlist'

function App() {
  const [currentCoin, setCurrentCoin] = useState('BTCUSDT');
  const [watchlist, setWatchlist] = useState(defaultWatchlist.coins);
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

      <Watchlist coins={watchlist}

        onAddCoin={handleAddCoin}

        onRemoveCoin={handleRemoveCoin}

        onCoin={(c) => setCurrentCoin(c)}

        currentCoin={currentCoin}
      />
      <ShowPrice price={price} coin={currentCoin} />
    </>
  )
}

export default App
