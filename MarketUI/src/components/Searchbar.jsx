import { useState } from 'react';


const SearchBar= ({onSearch}) => {
    // Capture the user data.
    const [query, setQuery] = useState("");
    
    function submitSearch() {
        const trimmed = query.trim();
        if (!trimmed) return;
        
        onSearch(trimmed);
        setQuery("");
        }
  
    // Maybe break this out so it is easier to see.
    const KeyDownHandler = (e) => {
        if (e.key === "Enter") {
            submitSearch();
        }
    };
    
    return(
      <div>
        <input
            value={query} 
            onChange={(e) => setQuery(e.target.value)}
            onKeyDown={KeyDownHandler}
        />
        <button onClick={() => {
          submitSearch();
        }}
        >search</button>
      </div>
    );
}

export default SearchBar