import '../css/navBar.css'


const Navbar = () => {

  return (
    <div className="navBar-wrapper-div">
      <nav className="navbar">
        <div className="navbar-logo">
          <h1>MARKET MACHINE</h1>
        </div>

      <div className="navbar-spacer" />

        <ul className="navbar-links">
          <li><a href="#">Home</a></li>
          <li><a href="#">About</a></li>
          <li><a href="#">Contact</a></li>
        </ul>
      </nav>
    </div>
  )
}

export default Navbar
