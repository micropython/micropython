/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { IDisposable, IEventEmitter, XtermListener } from './Types';
import { Disposable } from './Lifecycle';

export class EventEmitter extends Disposable implements IEventEmitter, IDisposable {
  private _events: {[type: string]: XtermListener[]};

  constructor() {
    super();
    // Restore the previous events if available, this will happen if the
    // constructor is called multiple times on the same object (terminal reset).
    this._events = (<any>this)._events || {};
  }

  public on(type: string, listener: XtermListener): void {
    this._events[type] = this._events[type] || [];
    this._events[type].push(listener);
  }

  /**
   * Adds a disposable listener to the EventEmitter, returning the disposable.
   * @param type The event type.
   * @param handler The handler for the listener.
   */
  public addDisposableListener(type: string, handler: XtermListener): IDisposable {
    // TODO: Rename addDisposableEventListener to more easily disambiguate from Dom listener
    this.on(type, handler);
    let disposed = false;
    return {
      dispose: () => {
        if (disposed) {
          // Already disposed
          return;
        }
        this.off(type, handler);
        disposed = true;
      }
    };
  }

  public off(type: string, listener: XtermListener): void {
    if (!this._events[type]) {
      return;
    }

    const obj = this._events[type];
    let i = obj.length;

    while (i--) {
      if (obj[i] === listener) {
        obj.splice(i, 1);
        return;
      }
    }
  }

  public removeAllListeners(type: string): void {
    if (this._events[type]) {
       delete this._events[type];
    }
  }

  public emit(type: string, ...args: any[]): void {
    if (!this._events[type]) {
      return;
    }
    const obj = this._events[type];
    for (let i = 0; i < obj.length; i++) {
      obj[i].apply(this, args);
    }
  }

  public emitMayRemoveListeners(type: string, ...args: any[]): void {
    if (!this._events[type]) {
      return;
    }
    const obj = this._events[type];
    let length = obj.length;
    for (let i = 0; i < obj.length; i++) {
      obj[i].apply(this, args);
      i -= length - obj.length;
      length = obj.length;
    }
  }

  public listeners(type: string): XtermListener[] {
    return this._events[type] || [];
  }

  public dispose(): void {
    super.dispose();
    this._events = {};
  }
}
