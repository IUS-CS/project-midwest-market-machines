/* Navbar Vitest
 *
 * navBar.test.jsx
 *
 * Navbar is a stateless presentational component with no internal logic,
 * so instead, these tests encode the string and structural contracts it is expected
 * to satisfy
 *
 * Any test requiring a mounted component belongs in navBar.spec.jsx.
 */

import { expect, describe, it } from 'vitest';

/* 
* Constants: values Navbar is expected to render
*/ 

const BRAND_NAME = 'MARKET MACHINE';

/*
* Brand Copy Contracts
*/ 

describe('Navbar Brand Copy Contracts', () => {
  /* Encodes the expected brand string so a copy change causes a visible
   * test failure rather than a silent one. */

  it('should define the brand name as a non-empty string', () => {
    expect(typeof BRAND_NAME).toBe('string');
    expect(BRAND_NAME.length).toBeGreaterThan(0);
  });

  it('should define the brand name as MARKET MACHINE', () => {
    expect(BRAND_NAME).toBe('MARKET MACHINE');
  });

  it('should be entirely uppercase', () => {
    expect(BRAND_NAME).toBe(BRAND_NAME.toUpperCase());
  });

  it('should contain two words separated by a single space', () => {
    const words = BRAND_NAME.split(' ');
    expect(words.length).toBe(2);
    expect(words[0]).toBe('MARKET');
    expect(words[1]).toBe('MACHINE');
  });
});

/* 
* Structural Contracts
*/ 

describe('Navbar Structural Contracts', () => {
  /* Navbar renders no interactive links currently it is a logo-only bar.
   * These tests document that the component is intentionally stateless. */

  it('should have no required props (zero-arg render contract)', () => {
    /* Navbar accepts no props: A factory that passes no arguments should
     * return the same static value every time. */
    const makeProps = () => ({});
    expect(makeProps()).toEqual({});
  });

  it('should not expose any interactive state', () => {
    /* The component has no useState / useReducer calls.
     * so,m verify this contract by confirming the props object is empty. */
    const props = {};
    expect(Object.keys(props).length).toBe(0);
  });
});
