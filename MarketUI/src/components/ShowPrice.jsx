import '../css/ShowPrice.css'

const ShowPrice = ({ price, coin }) => {

  return (
    <div className="ShowPrice-wrapper-div">
      <nav className="ShowPrice">
        <h3 className="PriceHeader">{coin}'s Price</h3>
        <h3 className="Price">${price}</h3>
      </nav>
      <h4 className="Disclaimer-of-Spaghetti-code">Ticker updates every 4 seconds.</h4>
    </div>
  )
}

export default ShowPrice
