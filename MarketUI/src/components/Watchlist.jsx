import { useState } from 'react';
import '../css/Watchlist.css';


// Watchlist component to display user's watchlist
const Watchlist = ({ coins,onAddCoin, onRemoveCoin, onCoin, currentCoin  }) => {
  const [newCoin, setNewCoin] = useState('');



  return (
    <div className="watchlist-wrapper-div">
      <nav className="watchlist">
        <h2>My Watchlist</h2>
        {coins.length === 0 ?(
            <p> Add your favorite coins to the list</p>
              ):
              (<ul>
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
                    <span>{coin.replace('USDT', '')}</span>
                    <button onClick={(e) => { e.stopPropagation(); onRemoveCoin(coin); }}>remove</button>
                  </li>
                ))}
              </ul>
             )}
        <input
            value={newCoin}
            onChange={(e) => setNewCoin(e.target.value)}
            onKeyDown={(e) => {
              if (e.key === "Enter") {
                onAddCoin(newCoin);
                setNewCoin('');
              }
            }}
            placeholder="e.g. SOLUSDT"
        />
        <button
        onClick={() => {
          onAddCoin(newCoin);
          setNewCoin('');
        }}
      >
        add
      </button>
      </nav>
    </div>
  );
}

export default Watchlist;
