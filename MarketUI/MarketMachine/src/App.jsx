import './App.css'
import Navbar from './components/navBar'
import Watchlist from './components/Watchlist'

function App() {


  return (
    <>
      <div className="navBar">
        <Navbar />
      </div>

      <div className="App_header">
        <h1>Simple Trade</h1>
        <p>Your one-stop shop for all things crypto!</p>
      </div>

      <div className="watchlist">
        <Watchlist />
      </div>
    </>
  )
}

export default App
