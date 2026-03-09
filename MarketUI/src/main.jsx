import { StrictMode } from 'react'
import { createRoot } from 'react-dom/client'
import './index.css'
import App from './App.jsx'
/*Entry point of the React application. Renders the App.jsx  component which contains
  the main structure of the application, including the navbar, watchlist, and price display.
*/
createRoot(document.getElementById('root')).render(
  // App in StrictMode to enable additional checks and warnings during development
  <StrictMode>
    <App />
  </StrictMode>,
)
