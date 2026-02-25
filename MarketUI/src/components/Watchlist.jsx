import { useState } from 'react';
import '../css/Watchlist.css'

// Watchlist component to display user's watchlist
const Watchlist = ({ onCoin, currentCoin, stock }) => {
  const [watchlist, setWatchlist] = useState([
    "BTCUSDT", "ETHUSDT", "ADAUSDT", "XRPUSDT", "DOTUSDT", "UNIUSDT"
  ]);

  const addToWatchlist = (coin) => {
    if (coin && !watchlist.includes(coin)) {
      setWatchlist([...watchlist, coin]);
    }
  };

  const removeFromWatchlist = (coin) => {
    setWatchlist(watchlist.filter((item) => item !== coin));
  };

  return (
    <div className="watchlist-wrapper-div">
      <nav className="watchlist">
        <h2>My Watchlist</h2>
        <ul>
          {watchlist.map((coin) => (
            <li
              key={coin}
              className={currentCoin === coin ? "active" : ""}
              onClick={() => onCoin(coin)}
              style={{ cursor: 'pointer' }}
            >
              <span>{coin.replace('USDT', '')}</span>
              <button onClick={(e) => { e.stopPropagation(); removeFromWatchlist(coin); }}>remove</button>
            </li>
          ))}
        </ul>
        <button onClick={() => { addToWatchlist(stock); }}>add</button>
      </nav>
    </div>
  );
}

export default Watchlist;
