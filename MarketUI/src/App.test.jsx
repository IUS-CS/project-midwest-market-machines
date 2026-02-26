import { expect, test, describe, it } from 'vitest'
import { WebSocket } from './WebSocket.jsx'

// Basic test - tests that tests work.
// Shows how to use a test in vitest.

test('1 + 1', () => {
  expect(1 + 1).toEqual(2)
});

describe('Testing Plan', () => {
  it.todo('should determine if ShowPrice updates on a new price');
  it.todo('should determine that each link is clickable');
  it.todo('should determine that serialized JSONs are received and parsed accurately');
})
