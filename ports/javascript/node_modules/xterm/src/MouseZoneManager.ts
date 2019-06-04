/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { ITerminal, IMouseZoneManager, IMouseZone } from './Types';
import { Disposable } from './common/Lifecycle';
import { addDisposableDomListener } from './ui/Lifecycle';

const HOVER_DURATION = 500;

/**
 * The MouseZoneManager allows components to register zones within the terminal
 * that trigger hover and click callbacks.
 *
 * This class was intentionally made not so robust initially as the only case it
 * needed to support was single-line links which never overlap. Improvements can
 * be made in the future.
 */
export class MouseZoneManager extends Disposable implements IMouseZoneManager {
  private _zones: IMouseZone[] = [];

  private _areZonesActive: boolean = false;
  private _mouseMoveListener: (e: MouseEvent) => any;
  private _mouseLeaveListener: (e: MouseEvent) => any;
  private _clickListener: (e: MouseEvent) => any;

  private _tooltipTimeout: number = null;
  private _currentZone: IMouseZone = null;
  private _lastHoverCoords: [number, number] = [null, null];
  private _initialSelectionLength: number;

  constructor(
    private _terminal: ITerminal
  ) {
    super();

    this.register(addDisposableDomListener(this._terminal.element, 'mousedown', e => this._onMouseDown(e)));

    // These events are expensive, only listen to it when mouse zones are active
    this._mouseMoveListener = e => this._onMouseMove(e);
    this._mouseLeaveListener = e => this._onMouseLeave(e);
    this._clickListener = e => this._onClick(e);
  }

  public dispose(): void {
    super.dispose();
    this._deactivate();
  }

  public add(zone: IMouseZone): void {
    this._zones.push(zone);
    if (this._zones.length === 1) {
      this._activate();
    }
  }

  public clearAll(start?: number, end?: number): void {
    // Exit if there's nothing to clear
    if (this._zones.length === 0) {
      return;
    }

    // Clear all if start/end weren't set
    if (!end) {
      start = 0;
      end = this._terminal.rows - 1;
    }

    // Iterate through zones and clear them out if they're within the range
    for (let i = 0; i < this._zones.length; i++) {
      const zone = this._zones[i];
      if ((zone.y1 > start && zone.y1 <= end + 1) ||
          (zone.y2 > start && zone.y2 <= end + 1) ||
          (zone.y1 < start && zone.y2 > end + 1)) {
        if (this._currentZone && this._currentZone === zone) {
          this._currentZone.leaveCallback();
          this._currentZone = null;
        }
        this._zones.splice(i--, 1);
      }
    }

    // Deactivate the mouse zone manager if all the zones have been removed
    if (this._zones.length === 0) {
      this._deactivate();
    }
  }

  private _activate(): void {
    if (!this._areZonesActive) {
      this._areZonesActive = true;
      this._terminal.element.addEventListener('mousemove', this._mouseMoveListener);
      this._terminal.element.addEventListener('mouseleave', this._mouseLeaveListener);
      this._terminal.element.addEventListener('click', this._clickListener);
    }
  }

  private _deactivate(): void {
    if (this._areZonesActive) {
      this._areZonesActive = false;
      this._terminal.element.removeEventListener('mousemove', this._mouseMoveListener);
      this._terminal.element.removeEventListener('mouseleave', this._mouseLeaveListener);
      this._terminal.element.removeEventListener('click', this._clickListener);
    }
  }

  private _onMouseMove(e: MouseEvent): void {
    // TODO: Ideally this would only clear the hover state when the mouse moves
    // outside of the mouse zone
    if (this._lastHoverCoords[0] !== e.pageX || this._lastHoverCoords[1] !== e.pageY) {
      this._onHover(e);
      // Record the current coordinates
      this._lastHoverCoords = [e.pageX, e.pageY];
    }
  }

  private _onHover(e: MouseEvent): void {
    const zone = this._findZoneEventAt(e);

    // Do nothing if the zone is the same
    if (zone === this._currentZone) {
      return;
    }

    // Fire the hover end callback and cancel any existing timer if a new zone
    // is being hovered
    if (this._currentZone) {
      this._currentZone.leaveCallback();
      this._currentZone = null;
      if (this._tooltipTimeout) {
        clearTimeout(this._tooltipTimeout);
      }
    }

    // Exit if there is not zone
    if (!zone) {
      return;
    }
    this._currentZone = zone;

    // Trigger the hover callback
    if (zone.hoverCallback) {
      zone.hoverCallback(e);
    }

    // Restart the tooltip timeout
    this._tooltipTimeout = <number><any>setTimeout(() => this._onTooltip(e), HOVER_DURATION);
  }

  private _onTooltip(e: MouseEvent): void {
    this._tooltipTimeout = null;
    const zone = this._findZoneEventAt(e);
    if (zone && zone.tooltipCallback) {
      zone.tooltipCallback(e);
    }
  }

  private _onMouseDown(e: MouseEvent): void {
    // Store current terminal selection length, to check if we're performing
    // a selection operation
    this._initialSelectionLength = this._terminal.getSelection().length;

    // Ignore the event if there are no zones active
    if (!this._areZonesActive) {
      return;
    }

    // Find the active zone, prevent event propagation if found to prevent other
    // components from handling the mouse event.
    const zone = this._findZoneEventAt(e);
    if (zone) {
      if (zone.willLinkActivate(e)) {
        e.preventDefault();
        e.stopImmediatePropagation();
      }
    }
  }

  private _onMouseLeave(e: MouseEvent): void {
    // Fire the hover end callback and cancel any existing timer if the mouse
    // leaves the terminal element
    if (this._currentZone) {
      this._currentZone.leaveCallback();
      this._currentZone = null;
      if (this._tooltipTimeout) {
        clearTimeout(this._tooltipTimeout);
      }
    }
  }

  private _onClick(e: MouseEvent): void {
    // Find the active zone and click it if found and no selection was
    // being performed
    const zone = this._findZoneEventAt(e);
    const currentSelectionLength = this._terminal.getSelection().length;

    if (zone && currentSelectionLength === this._initialSelectionLength) {
      zone.clickCallback(e);
      e.preventDefault();
      e.stopImmediatePropagation();
    }
  }

  private _findZoneEventAt(e: MouseEvent): IMouseZone {
    const coords = this._terminal.mouseHelper.getCoords(e, this._terminal.screenElement, this._terminal.charMeasure, this._terminal.cols, this._terminal.rows);
    if (!coords) {
      return null;
    }
    const x = coords[0];
    const y = coords[1];
    for (let i = 0; i < this._zones.length; i++) {
      const zone = this._zones[i];
      if (zone.y1 === zone.y2) {
        // Single line link
        if (y === zone.y1 && x >= zone.x1 && x < zone.x2) {
          return zone;
        }
      } else {
        // Multi-line link
        if ((y === zone.y1 && x >= zone.x1) ||
            (y === zone.y2 && x < zone.x2) ||
            (y > zone.y1 && y < zone.y2)) {
          return zone;
        }
      }
    }
    return null;
  }
}

export class MouseZone implements IMouseZone {
  constructor(
    public x1: number,
    public y1: number,
    public x2: number,
    public y2: number,
    public clickCallback: (e: MouseEvent) => any,
    public hoverCallback: (e: MouseEvent) => any,
    public tooltipCallback: (e: MouseEvent) => any,
    public leaveCallback: () => void,
    public willLinkActivate: (e: MouseEvent) => boolean
  ) {
  }
}
