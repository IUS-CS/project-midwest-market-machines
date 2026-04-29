import { expect, test } from 'vitest';
import ShowPrice from './ShowPrice.jsx';
/*
* Simple function to ensure logic of display component works, 
* no if/else logic means testing is trivial, linear cyclomatic or something I don't remember
*/ 
test('ShowPrice coverage at 100%', () => {
  const props = { coin: 'BTCUSDT', price: '76480.36' };
  const result = ShowPrice(props);
  expect(JSON.stringify(result)).toContain('BTCUSDT');
  expect(JSON.stringify(result)).toContain('76480.36');
});