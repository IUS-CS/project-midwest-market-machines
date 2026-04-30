/* Navbar Playwright Component Test
 *
 * navBar.spec.jsx
 *
 * Mounts Navbar into Playwright component testing environment and asserts on the
 * rendered DOM structure.
 */

import { test, expect } from '@playwright/experimental-ct-react';
import Navbar from '../src/components/navBar';

/*
* Static Render
*/

/* Confirms the component mounts without throwing. */
test('should render without crashing', async ({ mount }) => {
  const component = await mount(<Navbar />);
  await expect(component).toBeVisible();
});

/* Confirms the MARKET MACHINE brand text is present in the rendered output. */
test('should display the MARKET MACHINE logo text', async ({ mount }) => {
  const component = await mount(<Navbar />);
  await expect(component).toContainText('MARKET MACHINE');
});

/* Confirms a <nav> element is rendered with the correct class for semantic structure. */
test('should render a nav element with class navbar', async ({ mount }) => {
  const component = await mount(<Navbar />);
  // Target the internal <nav> element
  await expect(component.locator('nav.navbar')).toBeVisible();
});

/* Confirms the brand heading is an <h1> element. */
test('should render the brand name inside an h1 element', async ({ mount }) => {
  const component = await mount(<Navbar />);
  await expect(component.locator('h1')).toContainText('MARKET MACHINE');
});

/* Confirms the navbar wrapper div is the root element and has the correct class. */
test('should render the navBar-wrapper-div container', async ({ mount }) => {
  const component = await mount(<Navbar />);
  // Check the component itself for the class, rather than locating it inside
  await expect(component).toHaveClass('navBar-wrapper-div');
});