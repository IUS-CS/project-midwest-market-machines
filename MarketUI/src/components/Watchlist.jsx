import { useState } from 'react';
import '../css/Watchlist.css';
import defaultWatchlist from '../LocalStorage/defaultWatchlist';

// Watchlist component to display user's watchlist
const Watchlist = ({ onCoin, currentCoin, coin }) => {
  const [watchlist, setWatchlist] = useState(defaultWatchlist);

  // Function to add a coin to the watchlist
  const addToWatchlist = (coin) => {
    if (coin && !watchlist.includes(coin)) {
      setWatchlist([...watchlist, coin]);
    }
  };
  
  // Function to remove a coin from the watchlist
  const removeFromWatchlist = (coin) => {
    setWatchlist(watchlist.filter((item) => item !== coin));
  };

  return (
    <div className="watchlist-wrapper-div">
      <nav className="watchlist">
        <h2>My Watchlist</h2>
        {watchlist.length == 0 ?(
            <p> Add your favorite coins to the list</p>

       
              ):
              (<ul>
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
             )}
        <button onClick={() => { addToWatchlist(coin); }}>add</button>
      </nav>
    </div>
  );
}

export default Watchlist;
