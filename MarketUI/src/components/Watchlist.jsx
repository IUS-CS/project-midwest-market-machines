import { useState } from 'react';
import '../css/Watchlist.css'

// Watchlist component to display user's watchlist
const Watchlist = ({ onCoin, currentCoin }) => {
  const coins = ["BTCUSDT", "ETHUSDT", "ADAUSDT", "XRPUSDT", "DOTUSDT", "UNIUSDT"];

  return (
    <div className="watchlist-wrapper-div">
      <nav className="watchlist">
        <h2 className="text-2xl font-bold mb-4">My Watchlist</h2>
        <ul className="list-disc list-inside">
          {/* Maps through the coins array to generate a clickable list item dynamically for each coin. 
          Logic could be extended or merged to generate new coin charts.
          */}
          {coins.map((coin) => (
            <li
              key={coin}
              className={currentCoin === coin ? "active" : ""}
              onClick={() => onCoin(coin)}
              style={{ cursor: 'pointer' }}
            >
              {/* Removes the 'USDT' suffix for cleaner display in the watchlist */}
              {coin.replace('USDT', '')}
            </li>
          ))}
        </ul>
      </nav>
    </div>
  );
}

export default Watchlist;
