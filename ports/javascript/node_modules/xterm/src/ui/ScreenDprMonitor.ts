/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { Disposable } from '../common/Lifecycle';

export type ScreenDprListener = (newDevicePixelRatio?: number, oldDevicePixelRatio?: number) => void;

/**
 * The screen device pixel ratio monitor allows listening for when the
 * window.devicePixelRatio value changes. This is done not with polling but with
 * the use of window.matchMedia to watch media queries. When the event fires,
 * the listener will be reattached using a different media query to ensure that
 * any further changes will register.
 *
 * The listener should fire on both window zoom changes and switching to a
 * monitor with a different DPI.
 */
export class ScreenDprMonitor extends Disposable {
  private _currentDevicePixelRatio: number = window.devicePixelRatio;
  private _outerListener: ((this: MediaQueryList, ev: MediaQueryListEvent) => any) | undefined;
  private _listener: ScreenDprListener | undefined;
  private _resolutionMediaMatchList: MediaQueryList | undefined;

  public setListener(listener: ScreenDprListener): void {
    if (this._listener) {
      this.clearListener();
    }
    this._listener = listener;
    this._outerListener = () => {
      if (!this._listener) {
        return;
      }
      this._listener(window.devicePixelRatio, this._currentDevicePixelRatio);
      this._updateDpr();
    };
    this._updateDpr();
  }

  public dispose(): void {
    super.dispose();
    this.clearListener();
  }

  private _updateDpr(): void {
    if (!this._resolutionMediaMatchList || !this._outerListener) {
      return;
    }

    // Clear listeners for old DPR
    this._resolutionMediaMatchList.removeListener(this._outerListener);

    // Add listeners for new DPR
    this._currentDevicePixelRatio = window.devicePixelRatio;
    this._resolutionMediaMatchList = window.matchMedia(`screen and (resolution: ${window.devicePixelRatio}dppx)`);
    this._resolutionMediaMatchList.addListener(this._outerListener);
  }

  public clearListener(): void {
    if (!this._resolutionMediaMatchList || !this._listener || !this._outerListener) {
      return;
    }
    this._resolutionMediaMatchList.removeListener(this._outerListener);
    this._resolutionMediaMatchList = undefined;
    this._listener = undefined;
    this._outerListener = undefined;
  }
}
