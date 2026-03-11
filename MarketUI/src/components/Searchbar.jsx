import { useState } from 'react';


// this will be a search component to work with the 
//CandlestickChart component to update the chart as the UserActivation
//is search for different candles
// Primary goal is to collect the text input call 
//onSearch(query) when submitted

const SearchBar=({onSearch})=>{
    // capture the user data
    const [query,setQuery]=useState("");
    function submitSearch() {
        const trimmed = query.trim();
        if (!trimmed) return;
        onSearch(trimmed);
        setQuery("");
        }
  
    return(
      <div>
        <input value={query} 
        onChange={(e)=> setQuery(e.target.value)}
        onKeyDown={(e) => {
              if (e.key === "Enter") {
                submitSearch();
              }
           }}
        
        
        />
        <button onClick={() => {
          submitSearch();
        }}
        >search</button>
        
        
        
      </div>

    );
}

export default SearchBar