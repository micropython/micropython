/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { Terminal } from 'xterm';

/**
 *
 * Allow xterm.js to handle ZMODEM uploads and downloads.
 *
 * This addon is a wrapper around zmodem.js. It adds the following to the
 *  Terminal class:
 *
 * - function `zmodemAttach(<WebSocket>, <Object>)` - creates a Zmodem.Sentry
 *      on the passed WebSocket object. The Object passed is optional and
 *      can contain:
 *          - noTerminalWriteOutsideSession: Suppress writes from the Sentry
 *            object to the Terminal while there is no active Session. This
 *            is necessary for compatibility with, for example, the
 *            `attach.js` addon.
 *
 * - event `zmodemDetect` - fired on Zmodem.Sentry’s `on_detect` callback.
 *      Passes the zmodem.js Detection object.
 *
 * - event `zmodemRetract` - fired on Zmodem.Sentry’s `on_retract` callback.
 *
 * You’ll need to provide logic to handle uploads and downloads.
 * See zmodem.js’s documentation for more details.
 *
 * **IMPORTANT:** After you confirm() a zmodem.js Detection, if you have
 *  used the `attach` or `terminado` addons, you’ll need to suspend their
 *  operation for the duration of the ZMODEM session. (The demo does this
 *  via `detach()` and a re-`attach()`.)
 */

let zmodem: any;

export interface IZmodemOptions {
  noTerminalWriteOutsideSession?: boolean;
}

function zmodemAttach(ws: WebSocket, opts: IZmodemOptions = {}): void {
  const term = this;
  const senderFunc = (octets: ArrayLike<number>) => ws.send(new Uint8Array(octets));

  let zsentry: any;

  function shouldWrite(): boolean {
    return !!zsentry.get_confirmed_session() || !opts.noTerminalWriteOutsideSession;
  }

  zsentry = new zmodem.Sentry({
    to_terminal: (octets: ArrayLike<number>) => {
      if (shouldWrite()) {
        term.write(
          String.fromCharCode.apply(String, octets)
        );
      }
    },
    sender: senderFunc,
    on_retract: () => (<any>term).emit('zmodemRetract'),
    on_detect: (detection: any) => (<any>term).emit('zmodemDetect', detection)
  });

  function handleWSMessage(evt: MessageEvent): void {

    // In testing with xterm.js’s demo the first message was
    // always text even if the rest were binary. While that
    // may be specific to xterm.js’s demo, ultimately we
    // should reject anything that isn’t binary.
    if (typeof evt.data === 'string') {
      if (shouldWrite()) {
        term.write(evt.data);
      }
    }
    else {
      zsentry.consume(evt.data);
    }
  }

  ws.binaryType = 'arraybuffer';
  ws.addEventListener('message', handleWSMessage);
}

export function apply(terminalConstructor: typeof Terminal): void {
  zmodem = (typeof window === 'object') ? (<any>window).Zmodem : {Browser: null};  // Nullify browser for tests

  (<any>terminalConstructor.prototype).zmodemAttach = zmodemAttach;
  (<any>terminalConstructor.prototype).zmodemBrowser = zmodem.Browser;
}
