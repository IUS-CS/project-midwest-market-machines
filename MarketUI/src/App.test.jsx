import { expect, test, describe, it } from 'vitest';
import { render, screen } from '@testing-library/react';
import App from './App.jsx';

// Basic test - tests that tests work.
// Shows how to use a test in vitest.

test('1 + 1', () => {
  expect(1 + 1).toEqual(2)
});

describe('App Component', () => {
  it('renders without crashing', () => {
    render(<App />);
  });

  it.todo('should update the view when a new coin is selected');
});