/**
 * Copyright (c) 2014 The xterm.js authors. All rights reserved.
 * @license MIT
 *
 * Implements the attach method, that attaches the terminal to a WebSocket stream.
 */

import { Terminal, IDisposable } from 'xterm';
import { IAttachAddonTerminal } from './Interfaces';

/**
 * Attaches the given terminal to the given socket.
 *
 * @param term The terminal to be attached to the given socket.
 * @param socket The socket to attach the current terminal.
 * @param bidirectional Whether the terminal should send data to the socket as well.
 * @param buffered Whether the rendering of incoming data should happen instantly or at a maximum
 * frequency of 1 rendering per 10ms.
 */
export function attach(term: Terminal, socket: WebSocket, bidirectional: boolean, buffered: boolean): void {
  const addonTerminal = <IAttachAddonTerminal>term;
  bidirectional = (typeof bidirectional === 'undefined') ? true : bidirectional;
  addonTerminal.__socket = socket;

  addonTerminal.__flushBuffer = () => {
    addonTerminal.write(addonTerminal.__attachSocketBuffer);
    addonTerminal.__attachSocketBuffer = null;
  };

  addonTerminal.__pushToBuffer = (data: string) => {
    if (addonTerminal.__attachSocketBuffer) {
      addonTerminal.__attachSocketBuffer += data;
    } else {
      addonTerminal.__attachSocketBuffer = data;
      setTimeout(addonTerminal.__flushBuffer, 10);
    }
  };

  // TODO: This should be typed but there seem to be issues importing the type
  let myTextDecoder: any;

  addonTerminal.__getMessage = function(ev: MessageEvent): void {
    let str: string;

    if (typeof ev.data === 'object') {
      if (!myTextDecoder) {
        myTextDecoder = new TextDecoder();
      }
      if (ev.data instanceof ArrayBuffer) {
        str = myTextDecoder.decode(ev.data);
        displayData(str);
      } else {
        const fileReader = new FileReader();

        fileReader.addEventListener('load', () => {
          str = myTextDecoder.decode(fileReader.result);
          displayData(str);
        });
        fileReader.readAsArrayBuffer(ev.data);
      }
    } else if (typeof ev.data === 'string') {
      displayData(ev.data);
    } else {
      throw Error(`Cannot handle "${typeof ev.data}" websocket message.`);
    }
  };

  /**
  * Push data to buffer or write it in the terminal.
  * This is used as a callback for FileReader.onload.
  *
  * @param str String decoded by FileReader.
  * @param data The data of the EventMessage.
  */
  function displayData(str?: string, data?: string): void {
    if (buffered) {
      addonTerminal.__pushToBuffer(str || data);
    } else {
      addonTerminal.write(str || data);
    }
  }

  addonTerminal.__sendData = (data: string) => {
    if (socket.readyState !== 1) {
      return;
    }
    socket.send(data);
  };

  addonTerminal._core.register(addSocketListener(socket, 'message', addonTerminal.__getMessage));

  if (bidirectional) {
    addonTerminal.__dataListener = addonTerminal.onData(addonTerminal.__sendData);
    addonTerminal._core.register(addonTerminal.__dataListener);
  }

  addonTerminal._core.register(addSocketListener(socket, 'close', () => detach(addonTerminal, socket)));
  addonTerminal._core.register(addSocketListener(socket, 'error', () => detach(addonTerminal, socket)));
}

function addSocketListener(socket: WebSocket, type: string, handler: (this: WebSocket, ev: Event) => any): IDisposable {
  socket.addEventListener(type, handler);
  return {
    dispose: () => {
      if (!handler) {
        // Already disposed
        return;
      }
      socket.removeEventListener(type, handler);
      handler = null;
    }
  };
}

/**
 * Detaches the given terminal from the given socket
 *
 * @param term The terminal to be detached from the given socket.
 * @param socket The socket from which to detach the current terminal.
 */
export function detach(term: Terminal, socket: WebSocket): void {
  const addonTerminal = <IAttachAddonTerminal>term;
  addonTerminal.__dataListener.dispose();
  addonTerminal.__dataListener = undefined;

  socket = (typeof socket === 'undefined') ? addonTerminal.__socket : socket;

  if (socket) {
    socket.removeEventListener('message', addonTerminal.__getMessage);
  }

  delete addonTerminal.__socket;
}


export function apply(terminalConstructor: typeof Terminal): void {
  /**
   * Attaches the current terminal to the given socket
   *
   * @param socket The socket to attach the current terminal.
   * @param bidirectional Whether the terminal should send data to the socket as well.
   * @param buffered Whether the rendering of incoming data should happen instantly or at a maximum
   * frequency of 1 rendering per 10ms.
   */
  (<any>terminalConstructor.prototype).attach = function (socket: WebSocket, bidirectional: boolean, buffered: boolean): void {
    attach(this, socket, bidirectional, buffered);
  };

  /**
   * Detaches the current terminal from the given socket.
   *
   * @param socket The socket from which to detach the current terminal.
   */
  (<any>terminalConstructor.prototype).detach = function (socket: WebSocket): void {
    detach(this, socket);
  };
}
