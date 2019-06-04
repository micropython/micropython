/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { ITerminal, IBufferSet, IBuffer } from './Types';
import { IAttributeData } from './core/Types';
import { Buffer } from './Buffer';
import { EventEmitter2, IEvent } from './common/EventEmitter2';

/**
 * The BufferSet represents the set of two buffers used by xterm terminals (normal and alt) and
 * provides also utilities for working with them.
 */
export class BufferSet implements IBufferSet {
  private _normal: Buffer;
  private _alt: Buffer;
  private _activeBuffer: Buffer;


  private _onBufferActivate = new EventEmitter2<{activeBuffer: IBuffer, inactiveBuffer: IBuffer}>();
  public get onBufferActivate(): IEvent<{activeBuffer: IBuffer, inactiveBuffer: IBuffer}> { return this._onBufferActivate.event; }

  /**
   * Create a new BufferSet for the given terminal.
   * @param _terminal - The terminal the BufferSet will belong to
   */
  constructor(private _terminal: ITerminal) {
    this._normal = new Buffer(this._terminal, true);
    this._normal.fillViewportRows();

    // The alt buffer should never have scrollback.
    // See http://invisible-island.net/xterm/ctlseqs/ctlseqs.html#h2-The-Alternate-Screen-Buffer
    this._alt = new Buffer(this._terminal, false);
    this._activeBuffer = this._normal;

    this.setupTabStops();
  }

  /**
   * Returns the alt Buffer of the BufferSet
   */
  public get alt(): Buffer {
    return this._alt;
  }

  /**
   * Returns the normal Buffer of the BufferSet
   */
  public get active(): Buffer {
    return this._activeBuffer;
  }

  /**
   * Returns the currently active Buffer of the BufferSet
   */
  public get normal(): Buffer {
    return this._normal;
  }

  /**
   * Sets the normal Buffer of the BufferSet as its currently active Buffer
   */
  public activateNormalBuffer(): void {
    if (this._activeBuffer === this._normal) {
      return;
    }
    this._normal.x = this._alt.x;
    this._normal.y = this._alt.y;
    // The alt buffer should always be cleared when we switch to the normal
    // buffer. This frees up memory since the alt buffer should always be new
    // when activated.
    this._alt.clear();
    this._activeBuffer = this._normal;
    this._onBufferActivate.fire({
      activeBuffer: this._normal,
      inactiveBuffer: this._alt
    });
  }

  /**
   * Sets the alt Buffer of the BufferSet as its currently active Buffer
   */
  public activateAltBuffer(fillAttr?: IAttributeData): void {
    if (this._activeBuffer === this._alt) {
      return;
    }
    // Since the alt buffer is always cleared when the normal buffer is
    // activated, we want to fill it when switching to it.
    this._alt.fillViewportRows(fillAttr);
    this._alt.x = this._normal.x;
    this._alt.y = this._normal.y;
    this._activeBuffer = this._alt;
    this._onBufferActivate.fire({
      activeBuffer: this._alt,
      inactiveBuffer: this._normal
    });
  }

  /**
   * Resizes both normal and alt buffers, adjusting their data accordingly.
   * @param newCols The new number of columns.
   * @param newRows The new number of rows.
   */
  public resize(newCols: number, newRows: number): void {
    this._normal.resize(newCols, newRows);
    this._alt.resize(newCols, newRows);
  }

  /**
   * Setup the tab stops.
   * @param i The index to start setting up tab stops from.
   */
  public setupTabStops(i?: number): void {
    this._normal.setupTabStops(i);
    this._alt.setupTabStops(i);
  }
}
