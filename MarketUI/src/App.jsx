import { useState } from 'react'
import './App.css'
import Navbar from './components/navBar'
import Watchlist from './components/Watchlist'
import WebSocket from './WebSocket'
import ShowPrice from './components/ShowPrice'
import About from "./components/About"
import Contact from "./components/Contact"

function App() {
  const [currentCoin, setCurrentCoin] = useState('BTCUSDT');
  const { price } = WebSocket(currentCoin);

  const [page, setPage] = useState("home"); 

  return (
    <>
      <Navbar setPage={setPage} />

      {page === "home" && (
        <>
          <div className="App_header">
            <h1>Simple Trade</h1>
            <p>Your one-stop shop for all things crypto!</p>
          </div>

          {/* (2) User Input: Watchlist provides the interface for selecting a new coin */}
          {/* (3) State Update: onCoin triggers setCurrentCoin, restarting the cycle at step 1 */}
          <Watchlist onCoin={(c) => setCurrentCoin(c)} currentCoin={currentCoin} />
          {/* (5) Rendered Output: ShowPrice receives the final price and renders it to the screen */}
          <ShowPrice price={price} coin={currentCoin} />
        </>
      )}

      {page === "about" && <About />}
      {page === "contact" && <Contact />}
    </>
  )
}

export default App
