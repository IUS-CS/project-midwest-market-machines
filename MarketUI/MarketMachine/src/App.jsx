import './App.css'
import Navbar from './components/navBar'
import Watchlist from './components/Watchlist'

function App() {


  return (
    <>
      <Navbar />

      <div className="App_header">
        <h1>Simple Trade</h1>
        <p>Your one-stop shop for all things crypto!</p>
      </div>

      <Watchlist />
    </>
  )
}

export default App
