/* ShowPrice Playwright Component Test
 * Mounts ShowPrice into Playwright's CT environment and asserts on the
 * rendered DOM.  Logic unit and formatting tests live in ShowPrice.test.jsx
 *
 * Component contract:
 * props  : price (string | null), coin (string)
 * renders: "coin's Price" heading and price line
 */

import { test, expect } from '@playwright/experimental-ct-react';
import ShowPrice        from './ShowPrice';

/*
* Static Render
*/

/* Confirms the component mounts without throwing given well-formed props */
test('should render without crashing given valid props', async ({ mount }) => {
  const component = await mount(<ShowPrice price="72000.00" coin="BTCUSDT" />);
  await expect(component).toBeVisible();
});

/* Verifies the heading template "<Coin>'s Price" in the live DOM. */
test("should format the coin name as \"<Coin>'s Price\"", async ({ mount }) => {
  const component = await mount(<ShowPrice price="1800.00" coin="ETHUSDT" />);
  await expect(component).toContainText("ETHUSDT's Price");
});

/* Verifies the price is displayed with a $ prefix. */
test('should display the price with a $ prefix', async ({ mount }) => {
  const component = await mount(<ShowPrice price="500.00" coin="ADAUSDT" />);
  await expect(component).toContainText('$500.00');
});

/* Confirms both the coin heading and price value appear in the same render. */
test('should display both the coin heading and the price from props', async ({ mount }) => {
  const component = await mount(<ShowPrice price="72000.00" coin="BTCUSDT" />);
  await expect(component).toContainText('BTCUSDT');
  await expect(component).toContainText('72000');
});

/* Verifies the component survives a null price prop without throwing. */
test('should handle null price gracefully without crashing', async ({ mount }) => {
  await expect(mount(<ShowPrice price={null} coin="BTCUSDT" />)).resolves.toBeTruthy();
});

/*
* Prop-Change Re-renders
*/

/* Confirms the displayed price updates when the price prop changes,
* component.update() is used to simulate the App.jsx passing new props in
 *
 */
test('should update the displayed price when the price prop changes', async ({ mount }) => {
  const component = await mount(<ShowPrice price="72000.00" coin="BTCUSDT" />);
  
  await expect(component).toContainText('72000');

  // Trigger a re-render with a new price prop
  await component.update(<ShowPrice price="73500.00" coin="BTCUSDT" />);
  
  await expect(component).toContainText('73500');
});

/* Confirms the heading updates when the coin prop changes. 
 * We use component.update() to simulate the user switching to a different asset.
 */
test('should update the displayed coin heading when the coin prop changes', async ({ mount }) => {
  const component = await mount(<ShowPrice price="72000.00" coin="BTCUSDT" />);
  
  await expect(component).toContainText("BTCUSDT's Price");

  await component.update(<ShowPrice price="72000.00" coin="ETHUSDT" />);
  
  await expect(component).toContainText("ETHUSDT's Price");
});