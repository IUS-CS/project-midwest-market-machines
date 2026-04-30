import { test, expect } from '@playwright/experimental-ct-react';
import { AppMockup }    from './AppMockup';

/**
 * Static rendering tests, verifying that components are inside the DOM correctly
 */

test('should render without crashing', async ({ mount }) => {
  const component = await mount(<AppMockup />);
  await expect(component).toBeVisible();
});

test('should render the Simple Trade heading', async ({ mount }) => {
  const component = await mount(<AppMockup />);
  await expect(component).toContainText('Simple Trade');
});

test('should render the Watchlist with default coins', async ({ mount }) => {
  const component = await mount(<AppMockup />);
  await expect(component).toContainText('BTC');
  await expect(component).toContainText('ETH');
});

test('should render the TEST BUY and TEST SELL buttons', async ({ mount }) => {
  const component = await mount(<AppMockup />);
  await expect(component).toContainText('TEST BUY');
  await expect(component).toContainText('TEST SELL');
});

test('should render the ChatBox', async ({ mount }) => {
  const component = await mount(<AppMockup />);
  await expect(component.getByTestId('chatbox-stub')).toBeAttached();
});

test('should render the PurchasesTable with the correct holdings count', async ({ mount }) => {
  const holdings = [{ coin: 'BTC', quantity: 0.5 }, { coin: 'ETH', quantity: 1.0 }];
  const component = await mount(<AppMockup hookState={{ holdings }} />);
  await expect(component.getByTestId('purchases-stub')).toContainText('2 rows');
});

/**
 * Coin selection states, 
 */

test('should update the coin view when a new coin is selected', async ({ mount }) => {
  const component = await mount(<AppMockup initialCoin="BTCUSDT" />);
  await expect(component.getByTestId('candlestick-stub')).toContainText('BTCUSDT');

  await component.getByText('ETH').click();
  await expect(component.getByTestId('candlestick-stub')).toContainText('ETHUSDT');
});

test("should update ShowPrice's coin heading when a new coin is selected", async ({ mount }) => {
  const component = await mount(<AppMockup initialCoin="BTCUSDT" />);
  await expect(component).toContainText("BTCUSDT's Price");

  await component.getByText('ETH').click();
  await expect(component).toContainText("ETHUSDT's Price");
});

test("should pass the hook's price to ShowPrice", async ({ mount }) => {
  const component = await mount(<AppMockup hookState={{ price: '99999.99' }} />);
  await expect(component).toContainText('99999.99');
});

/**
 * Tests the clicking implementation for the buttons
 */

test('should send trade("buy", 1.0) when TEST BUY is clicked', async ({ mount }) => {
  let captured = null;
  const component = await mount(
    <AppMockup hookState={{ trade: (type, qty) => { captured = { type, qty }; } }} />
  );
  await component.getByText('TEST BUY').click();
  expect(captured).toEqual({ type: 'buy', qty: 1.0 });
});

test('should send trade("sell", 1.0) when TEST SELL is clicked', async ({ mount }) => {
  let captured = null;
  const component = await mount(
    <AppMockup hookState={{ trade: (type, qty) => { captured = { type, qty }; } }} />
  );
  await component.getByText('TEST SELL').click();
  expect(captured).toEqual({ type: 'sell', qty: 1.0 });
});