import '../css/ShowPrice.css'

/* Small component to display the price of a selected coin or asset */

const ShowPrice = ({ price, coin }) => {

  return (
    <div className="ShowPrice-wrapper-div">
      <nav className="ShowPrice">
        <h3 className="font-bold">{coin}'s Price</h3>
        <h3 className="font-bold">${price}</h3>
      </nav>
    </div>
  )
}

export default ShowPrice
