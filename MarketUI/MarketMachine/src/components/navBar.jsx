import '../css/navBar.css'


// Navbar component
const Navbar=()=>{

    return (
        <nav className="navbar">
            <div className="navbar-logo">
                <h1>Market Machine</h1>
            </div>
            <ul className="navbar-links">
                <li><a href="#">Home</a></li>
                <li><a href="#">About</a></li>
                <li><a href="#">Contact</a></li>
            </ul>
        </nav>
    )
}

export default Navbar