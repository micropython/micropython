/**
 * Copyright (c) 2018 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { IDisposable } from '../common/Types';

/**
 * Adds a disposable listener to a node in the DOM, returning the disposable.
 * @param type The event type.
 * @param handler The handler for the listener.
 */
export function addDisposableDomListener(
  node: Element | Window | Document,
  type: string,
  handler: (e: any) => void,
  useCapture?: boolean
): IDisposable {
  node.addEventListener(type, handler, useCapture);
  return {
    dispose: () => {
      if (!handler) {
        // Already disposed
        return;
      }
      node.removeEventListener(type, handler, useCapture);
    }
  };
}
