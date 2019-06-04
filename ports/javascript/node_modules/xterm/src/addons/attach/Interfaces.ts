/**
 * Copyright (c) 2018 The xterm.js authors. All rights reserved.
 * @license MIT
 *
 * Implements the attach method, that attaches the terminal to a WebSocket stream.
 */

import { Terminal, IDisposable } from 'xterm';

export interface IAttachAddonTerminal extends Terminal {
  _core: {
    register<T extends IDisposable>(d: T): void;
  };

  __socket?: WebSocket;
  __attachSocketBuffer?: string;
  __dataListener?: IDisposable;

  __getMessage?(ev: MessageEvent): void;
  __flushBuffer?(): void;
  __pushToBuffer?(data: string): void;
  __sendData?(data: string): void;
}
