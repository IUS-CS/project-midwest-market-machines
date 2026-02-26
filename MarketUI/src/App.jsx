import { useState } from 'react'
import './App.css'
import Navbar from './components/navBar'
import Watchlist from './components/Watchlist'
import WebSocket from './WebSocket'
import ShowPrice from './components/ShowPrice'

function App() {
  const [currentCoin, setCurrentCoin] = useState('BTCUSDT');
  const { price } = WebSocket(currentCoin);


  return (
    <>
      <Navbar />

      <div className="App_header">
        <h1>Simple Trade</h1>
        <p>Your one-stop shop for all things crypto!</p>
      </div>

      <Watchlist onCoin={(c) => setCurrentCoin(c)} currentCoin={currentCoin} />
      <ShowPrice price={price} coin={currentCoin} />
    </>
  )
}

export default App
