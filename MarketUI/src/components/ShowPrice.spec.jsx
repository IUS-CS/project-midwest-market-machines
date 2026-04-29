import { test, expect } from '@playwright/experimental-ct-react';
import ShowPrice from './ShowPrice';

test('ShowPrice should render the coin and price correctly', async ({ mount }) => {
  // 1. Mount the component into the Playwright "mini-app"
  const component = await mount(
    <ShowPrice coin="BTCUSDT" price="76480.36" />
  );

  // 2. Verify the visual output
  await expect(component).toContainText("BTCUSDT's Price");
  await expect(component).toContainText("$76480.36");
});