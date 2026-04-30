import '../css/navBar.css'

/* Navigation component with a logo and placeholder links for about, home, contact*/
const Navbar = () => {

  return (
    <div className="navBar-wrapper-div">
      <nav className="navbar">
        <div className="navbar-logo">
          <h1>MARKET MACHINE</h1>
        </div>
        {/* Spacer div to push the links to the right side of the navbar */}
        <div className="navbar-spacer" />
      </nav>
    </div>
  )
}

export default Navbar
