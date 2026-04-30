import { expect, test, describe, it } from 'vitest';

// Basic test - tests that tests work.
test('1 + 1', () => {
  expect(1 + 1).toEqual(2);
});

describe('App Component', () => {
  it.todo('renders without crashing');
  it.todo('should update the view when a new coin is selected');
  it.todo('should determine that serialized JSONs are received and parsed accurately');
});