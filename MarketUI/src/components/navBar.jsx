import '../css/navBar.css'

/* Navigation component with a logo and placeholder links for about, home, contact*/
const Navbar = ({ setPage }) => {

  return (
    <div className="navBar-wrapper-div">
      <nav className="navbar">
        <div className="navbar-logo">
          <h1>MARKET MACHINE</h1>
        </div>
    {/* Spacer div to push the links to the right side of the navbar */}
      <div className="navbar-spacer" />

        <ul className="navbar-links">
          <li><a href="#" onClick={() => setPage("home")}>Home</a></li>
          <li><a href="#" onClick={() => setPage("about")}>About</a></li>
          <li><a href="#" onClick={() => setPage("contact")}>Contact</a></li>
        </ul>
      </nav>
    </div>
  )
}

export default Navbar
