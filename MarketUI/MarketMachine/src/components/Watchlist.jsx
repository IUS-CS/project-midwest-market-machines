import { useState } from 'react';
import '../css/Watchlist.css'


// Watchlist component to display user's watchlist
const Watchlist = () => {
  const [watchlist, setWatchlist] = useState(["BTC", "ETH",
    "ADA", "XRP", "DOT", "UNI"]);

  return (
    <div className="watchlist-wrapper-div">
      <nav className="watchlist">
        <h2>My Watchlist</h2>
        <ul>
          {watchlist.map((item, index) => (
            <li key={index}>{item}</li>
          ))}
        </ul>
      </nav>
    </div>
  );

}

export default Watchlist;
