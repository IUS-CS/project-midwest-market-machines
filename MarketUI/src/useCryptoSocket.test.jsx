import { expect, test, describe, it } from 'vitest'
import { renderHook } from '@testing-library/react';
import useCryptoSocket from './useCryptoSocket.jsx';

describe('useCryptoSocket Hook', () => {
  it('should initialize with null data', () => {
    const { result } = renderHook(() => useCryptoSocket('BTCUSDT'));
    expect(result.current.price).toBeNull();
  });

  it.todo('should update price and latestCandle when receiving socket messages');
});