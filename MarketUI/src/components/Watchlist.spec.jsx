/* Watchlist Playwright Component Test
 * The Simple Trade App
 *
 * Watchlist.spec.jsx
 *
 * Mounts Watchlist into Playwright's CT environment and asserts on the
 * rendered DOM, click callbacks, and active-class behaviour.
 *
 * Component contract:
 * props  : onCoin (callback), currentCoin (string)
 * renders: "My Watchlist" heading, six clickable coin <li> items with
 * USDT stripped; active class applied to the currentCoin item
 */

import { test, expect } from '@playwright/experimental-ct-react';
import Watchlist         from '../components/Watchlist';

/*
/ Static Render
*/

/* Confirms the component mounts without throwing. */
test('should render without crashing', async ({ mount }) => {
  const component = await mount(<Watchlist onCoin={() => {}} currentCoin="BTCUSDT" />);
  await expect(component).toBeVisible();
});

test('should render the My Watchlist heading', async ({ mount }) => {
  const component = await mount(<Watchlist onCoin={() => {}} currentCoin="BTCUSDT" />);
  await expect(component).toContainText('My Watchlist');
});

/* Confirms all six default coins appear with USDT stripped from the label. */
test('should render all six default coins without the USDT suffix', async ({ mount }) => {
  const component = await mount(<Watchlist onCoin={() => {}} currentCoin="BTCUSDT" />);
  for (const label of ['BTC', 'ETH', 'ADA', 'XRP', 'DOT', 'UNI']) {
    await expect(component).toContainText(label);
  }
});

/*
* onCoimn
*/ 

/* Clicking ETH should fire onCoin with the full ticker 'ETHUSDT'. */
test('should call onCoin with the full ticker when a coin is clicked', async ({ mount }) => {
  let received = null;
  const component = await mount(
    <Watchlist onCoin={(c) => { received = c; }} currentCoin="BTCUSDT" />
  );
  await component.getByText('ETH').click();
  expect(received).toBe('ETHUSDT');
});

/* Clicking BTC should fire onCoin with 'BTCUSDT'. */
test('should call onCoin with BTCUSDT when BTC is clicked', async ({ mount }) => {
  let received = null;
  const component = await mount(
    <Watchlist onCoin={(c) => { received = c; }} currentCoin="ETHUSDT" />
  );
  await component.getByText('BTC').click();
  expect(received).toBe('BTCUSDT');
});

/* The currently selected coin's <li> must carry the active CSS class. */
test('should apply the active class to the currently selected coin', async ({ mount }) => {
  const component = await mount(<Watchlist onCoin={() => {}} currentCoin="ETHUSDT" />);
  const activeItem = component.getByText('ETH');
  await expect(activeItem).toHaveClass(/active/);
});

/* Non-selected coins must not carry the active class. */
test('should not apply the active class to non-selected coins', async ({ mount }) => {
  const component = await mount(<Watchlist onCoin={() => {}} currentCoin="ETHUSDT" />);
  // Removed .locator('..') because getByText targets the <li> directly
  const inactiveItem = component.getByText('BTC');
  await expect(inactiveItem).not.toHaveClass(/active/);
});

/* Active class should move to the new coin when currentCoin prop changes. */
test('should update the active class when currentCoin prop changes', async ({ mount }) => {
  const component = await mount(
    <Watchlist onCoin={() => {}} currentCoin="BTCUSDT" />
  );
  
  // Verify initial active state (removed .locator('..'))
  await expect(component.getByText('BTC')).toHaveClass(/active/);

  // Update props to simulate parent state change
  await component.update(
    <Watchlist onCoin={() => {}} currentCoin="ADAUSDT" />
  );

  await expect(component.getByText('ADA')).toHaveClass(/active/);
  await expect(component.getByText('BTC')).not.toHaveClass(/active/);
});