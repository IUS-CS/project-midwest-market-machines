import {useState} from 'react';



// Watchlist component to display user's watchlist
const Watchlist = () => {
    const [watchlist, setWatchlist] = useState(["BTC", "ETH",
        "ADA","XRP","DOT","UNI"]);

    return (
        <div className="watchlist">
            <h2>My Watchlist</h2>
            <ul>
                {watchlist.map((item, index) => (
                    <li key={index}>{item}</li>
                ))}
            </ul>
        </div>
    );

}

export default Watchlist;