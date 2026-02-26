import { useState } from 'react';
import '../css/Watchlist.css'

// Watchlist component to display user's watchlist
const Watchlist = ({ onCoin, currentCoin }) => {
  const coins = ["BTCUSDT", "ETHUSDT", "ADAUSDT", "XRPUSDT", "DOTUSDT", "UNIUSDT"];

  return (
    <div className="watchlist-wrapper-div">
      <nav className="watchlist">
        <h2>My Watchlist</h2>
        <ul>
          {coins.map((coin) => (
            <li
              key={coin}
              className={currentCoin === coin ? "active" : ""}
              onClick={() => onCoin(coin)}
              style={{ cursor: 'pointer' }}
            >
              {coin.replace('USDT', '')}
            </li>
          ))}
        </ul>
      </nav>
    </div>
  );
}

export default Watchlist;
