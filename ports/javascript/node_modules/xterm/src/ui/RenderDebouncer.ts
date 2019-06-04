/**
 * Copyright (c) 2018 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { IDisposable } from '../common/Types';

/**
 * Debounces calls to render terminal rows using animation frames.
 */
export class RenderDebouncer implements IDisposable {
  private _rowStart: number | undefined;
  private _rowEnd: number | undefined;
  private _rowCount: number | undefined;
  private _animationFrame: number | undefined;

  constructor(
    private _renderCallback: (start: number, end: number) => void
  ) {
  }

  public dispose(): void {
    if (this._animationFrame) {
      window.cancelAnimationFrame(this._animationFrame);
      this._animationFrame = undefined;
    }
  }

  public refresh(rowStart: number, rowEnd: number, rowCount: number): void {
    this._rowCount = rowCount;
    // Get the min/max row start/end for the arg values
    rowStart = rowStart !== undefined ? rowStart : 0;
    rowEnd = rowEnd !== undefined ? rowEnd : this._rowCount - 1;
    // Set the properties to the updated values
    this._rowStart = this._rowStart !== undefined ? Math.min(this._rowStart, rowStart) : rowStart;
    this._rowEnd = this._rowEnd !== undefined ? Math.max(this._rowEnd, rowEnd) : rowEnd;

    if (this._animationFrame) {
      return;
    }

    this._animationFrame = window.requestAnimationFrame(() => this._innerRefresh());
  }

  private _innerRefresh(): void {
    // Make sure values are set
    if (this._rowStart === undefined || this._rowEnd === undefined || this._rowCount === undefined) {
      return;
    }

    // Clamp values
    this._rowStart = Math.max(this._rowStart, 0);
    this._rowEnd = Math.min(this._rowEnd, this._rowCount - 1);

    // Run render callback
    this._renderCallback(this._rowStart, this._rowEnd);

    // Reset debouncer
    this._rowStart = undefined;
    this._rowEnd = undefined;
    this._animationFrame = undefined;
  }
}
