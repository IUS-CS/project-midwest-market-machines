import '../css/ShowPrice.css'

const ShowPrice = ({ price }) => {

  return (
    <div className="ShowPrice-wrapper-div">
      <nav className="ShowPrice">
        <h3 className="PriceHeader">Current Coin's Price</h3>
        <h3 className="Price">${price}</h3>
      </nav>
    </div>
  )
}

export default ShowPrice
