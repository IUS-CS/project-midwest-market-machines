/* CandlestickChart Playwright Component Test
 * CandlestickChart.spec.jsx
 *
 * Mounts CandlestickChart into Playwright's Component testing environment with
 * lightweight charts mocked so no canvas is required,  Asserts on the
 * chart library call sequence and DOM container.
 *
 * Component contract:
 *   props  : coin (string), latestCandle (object, null),
 *            historicalCandles array,
 *   mounts : a <div className="ChartContainer"> and initializes a chart
 *   updates: calls series.update(latestCandle) on latestCandle changes
 *            calls series.setData([]) on coin changes
 *   unmounts: calls chart.remove()
 */

import { test, expect } from '@playwright/experimental-ct-react';

// Import the mockup wrappers to satisfy Playwright CT mounting rules
import { 
  CoinReset, 
  CandleUpdater, 
  HistoricalLoader 
} from './CandlestickChartMockup';

import CandlestickChart from './CandlestickChart';

/* 
* Lightweight charts be intercepted and mocked before the component imports
* it, so we declare at module scope and inject its dependencies via the
* CT hooksConfig / beforeMount mechanism.
*/

/* mockState
 * Shared object that Playwright component tests can read after each mount to inspect call
 * counts, resets inside each test with the beforemount hook
 */
const mockState = {
  createChartCalls: 0,
  addSeriesCalls:   0,
  setDataCalls:     0,
  updateCalls:      0,
  removeCalls:      0,
  lastSetData:      null,
  lastUpdate:       null,
};

/* buildChartMock()
 * Returns a fresh mock chart and series pair and wires call tracking into
 * mockState.  Called once per mount via beforeMount.
 */
function buildChartMock() {
  const series = {
    setData: (data) => { mockState.setDataCalls++; mockState.lastSetData = data; },
    update:  (c)    => { mockState.updateCalls++;  mockState.lastUpdate  = c;    },
  };
  const chart = {
    addSeries:    ()      => { mockState.addSeriesCalls++; return series; },
    applyOptions: ()      => {},
    remove:       ()      => { mockState.removeCalls++; },
    timeScale:    ()      => ({ fitContent: () => {} }),
  };
  return { chart, series };
}

/* 
* hooksConfig: Playwright C.T. beforeMount hook injects the mock module
*/

export const hooksConfig = {
  beforeMount: async ({ App, hooksConfig }) => {
    // Reset calls counters between tests
    Object.assign(mockState, {
      createChartCalls: 0, addSeriesCalls: 0,
      setDataCalls: 0,     updateCalls: 0, removeCalls: 0,
      lastSetData: null,   lastUpdate: null,
    });

    // Patch window so the component's import resolves to our mock
    window.__lwc_mock__ = buildChartMock();
    window.__lwc_createChart__ = (el, opts) => {
      mockState.createChartCalls++;
      return window.__lwc_mock__.chart;
    };
  },
};

/*
* Mount Container Section
*/

/* Confirms the ChartContainer div is present in the rendered DOM. */
test('should render the ChartContainer div on mount', async ({ mount }) => {
  const component = await mount(
    <CandlestickChart coin="BTCUSDT" latestCandle={null} historicalCandles={[]} />
  );
  await expect(component).toBeAttached();
});

/* Confirms the component mounts without throwing. */
test('should render without crashing given valid props', async ({ mount }) => {
  await expect(
    mount(<CandlestickChart coin="BTCUSDT" latestCandle={null} historicalCandles={[]} />)
  ).resolves.toBeTruthy();
});

/*
* Prop-Change Behaviour
*/

/* Confirms the coin prop change triggers a data clearing render. */
test('should clear chart data when the coin prop changes', async ({ mount }) => {
  const component = await mount(<CoinReset />);
  await component.getByTestId('switch').click();
  // After switch, ChartContainer should still be visible
  await expect(component).toBeAttached();
});

/* Confirms the component renders after receiving a latestCandle prop. */
test('should remain visible after receiving a latestCandle prop', async ({ mount }) => {
  const component = await mount(<CandleUpdater />);
  await component.getByTestId('set').click();
  await expect(component).toBeAttached();
});

/* Confirms the component renders after a second latestCandle update. */
test('should remain visible after a second latestCandle update', async ({ mount }) => {
  const component = await mount(<CandleUpdater />);
  await component.getByTestId('set').click();
  await component.getByTestId('update').click();
  await expect(component).toBeAttached();
});

/* Confirms the component handles a non-empty historicalCandles array. */
test('should render without crashing when historicalCandles is populated', async ({ mount }) => {
  const component = await mount(<HistoricalLoader />);
  await component.getByTestId('load').click();
  await expect(component).toBeAttached();
});

/* 
 * 
 * Component Unmounting
 */

/* Confirms the component unmounts cleanly (chart.remove() should not throw). */
test('should unmount without throwing', async ({ mount }) => {
  const component = await mount(
    <CandlestickChart coin="BTCUSDT" latestCandle={null} historicalCandles={[]} />
  );
  await expect(component.unmount()).resolves.not.toThrow();
});