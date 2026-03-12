import { useState } from 'react';

// Search component that works with the CandlestickChart component
// to update the chart as the user searches for different candles.
// Collects text input and calls onSearch(query) when submitted.

const SearchBar = ({ onSearch }) => {
    const [query, setQuery] = useState("");

    function submitSearch() {
        const trimmed = query.trim();
        if (!trimmed) return;
        onSearch(trimmed);
        setQuery("");
    }

    const KeyDownHandler = (e) => {
        if (e.key === "Enter") {
            submitSearch();
        }
    };

    return (
        <div>
            <input
                value={query}
                onChange={(e) => setQuery(e.target.value)}
                onKeyDown={KeyDownHandler}
            />
            <button onClick={submitSearch}>search</button>
        </div>
    );
};

export default SearchBar;