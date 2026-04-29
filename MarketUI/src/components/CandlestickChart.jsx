import { useEffect, useRef } from 'react';
import * as LightweightCharts from 'lightweight-charts';
import '../css/CandlestickChart.css';

/*
    Updates and manages the life cycle of the chart.
*/

const CandlestickChart = ({ coin, latestCandle, historicalCandles }) => {
  const chartContainerRef = useRef(null);
  const chartRef = useRef(null);
  const seriesRef = useRef(null);
  const lastCoinRef = useRef(null);
  const dataLoadedRef = useRef(false);

  // Create the chart once on mount
  useEffect(() => {
    if (!chartContainerRef.current) return;

    const chart = LightweightCharts.createChart(chartContainerRef.current, {
      width: chartContainerRef.current.clientWidth,
      height: 400,
      layout: {
        background: { color: '#0a0a0a' },
        textColor: '#d1d4dc',
      },
      grid: {
        vertLines: { color: '#1a1a1a' },
        horzLines: { color: '#1a1a1a' },
      },
      timeScale: {
        visible: true,
        timeVisible: true,
        secondsVisible: false,
        borderColor: '#2a2a2a',
      },
      rightPriceScale: {
        autoScale: true,
        borderColor: '#2a2a2a',
        scaleMargins: { top: 0.1, bottom: 0.1 },
      },
      crosshair: {
        mode: LightweightCharts.CrosshairMode.Normal,
      },
    });

    seriesRef.current = chart.addSeries(LightweightCharts.CandlestickSeries, {
      upColor: '#26a69a',
      downColor: '#ef5350',
      borderUpColor: '#26a69a',
      borderDownColor: '#ef5350',
      wickUpColor: '#26a69a',
      wickDownColor: '#ef5350',
      borderVisible: true,
    });

    chartRef.current = chart;

    const handleResize = () => {
      if (chartRef.current && chartContainerRef.current) {
        chartRef.current.applyOptions({
          width: chartContainerRef.current.clientWidth,
        });
      }
    };
    window.addEventListener('resize', handleResize);

    return () => {
      window.removeEventListener('resize', handleResize);
      if (chartRef.current) {
        chartRef.current.remove();
        chartRef.current = null;
        seriesRef.current = null;
      }
    };
  }, []);

  /* Updates the chart with the data for the currently selected coin.
   * We update incrementally, unless the coin name changes or the histroical data
   * has JUST arrived.
   * 
   * 1. If we get an update that is *not* for the current coin, return.
   * 2. Get a bool 'coinChanged' that tracks whether the coin changed or not.
   * 2. If the coin changes...
   *    2a. Set dataLoadedRef = false;
   *    2b. Update the lastCoinRef's current to be this coin.
   * 3. If the coin changed, or dataLoadedRef is false and we have historical data...
   *    3a. Set the chart data from historicalCandles or an empty array if none.
   *    3b. If we have historical data...
   *        1. Adjust the chart timescale to fit.
   *        2. Set dataLoadedRef = true;
   *    3c. If coin changes or we have loaded data... return.
   * 4. If we have a latestCandle and it has a valid time...
   *    4a. Try-catch to update the chart.
   *    4b. Print a warning w/ error message if it failed.
  */
  useEffect(() => {
    if (!seriesRef.current) return;

    const coinChanged = lastCoinRef.current !== coin;

    if (coinChanged) {
      dataLoadedRef.current = false;
      lastCoinRef.current = coin;
    }

    // Check if the coin changed, or we're doing our first setData call.
    if (coinChanged || (!dataLoadedRef.current && historicalCandles.length > 0)) {
      seriesRef.current.setData(historicalCandles || []);

      if (historicalCandles.length > 0) {
        chartRef.current?.timeScale().fitContent();
        dataLoadedRef.current = true;
      }

      if (coinChanged || dataLoadedRef.current) return;
    }

    if (latestCandle && latestCandle.time) {
      try {
        seriesRef.current.update(latestCandle);
      } catch (err) {
        console.warn('[Chart] Live update failed:', err.message);
      }
    }
  }, [coin, latestCandle, historicalCandles]);

  return <div ref={chartContainerRef} className="ChartContainer" />;
};

export default CandlestickChart;