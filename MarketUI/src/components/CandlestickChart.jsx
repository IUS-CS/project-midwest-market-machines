import { useEffect, useRef } from 'react';
import * as LightweightCharts from 'lightweight-charts';
import '../css/CandlestickChart.css';

/*
    Updates and manages the life cycle of the chart.
*/

const CandlestickChart = ({ coin, latestCandle}) => {
    const chartContainerRef = useRef(null);
    const chartRef = useRef(null);
    const seriesRef = useRef(null);

    // Create the chart once on mount
    useEffect(() => {
        if (!chartContainerRef.current) return;

        const chart= LightweightCharts.createChart(chartContainerRef.current, {
            width: chartContainerRef.current.clientWidth,
            height: 400,
            layout: {
                background: {color: '#0a0a0a'},
                textColor: '#d1d4dc',
            },
            grid: {
                vertLines: {color: '#1a1a1a'},
                horzLines: {color: '#1a1a1a'},
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
                scaleMargins: {top: 0.1, bottom: 0.1},
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


    // Resets the chart when changing coin
    useEffect(() => {
        if (!seriesRef.current) return;

        seriesRef.current.setData([])

        // Not necessary until we have a REST API call to pre-load historical candles

        /* if (historicalCandles && historicalCandles.length > 0) {
            try {
                seriesRef.current.setData(historicalCandles);
                chartRef.current?.timeScale().fitContent();
            } catch (err) {
                console.error('[CandlestickChart] setData error on coin change:', err);
            }
        } else {
            seriesRef.current.setData([]);
        } */
    }, [coin]);

    // Appends candles when they have new timestamps, updates the candle if not
    useEffect(() => {
        if (!seriesRef.current || !latestCandle) return;

        try {
            seriesRef.current.update(latestCandle);
        } catch (err) {
            console.warn('[CandlestickChart] update() skipped (stale candle on coin switch):', err.message)
        }
    }, [latestCandle]);
 
    return <div ref={chartContainerRef} className="ChartContainer" />;
};

export default CandlestickChart;
