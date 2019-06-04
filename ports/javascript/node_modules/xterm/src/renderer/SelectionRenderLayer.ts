/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { ITerminal } from '../Types';
import { IRenderDimensions } from './Types';
import { BaseRenderLayer } from './BaseRenderLayer';
import { IColorSet } from '../ui/Types';

interface ISelectionState {
  start: [number, number];
  end: [number, number];
  columnSelectMode: boolean;
  ydisp: number;
}

export class SelectionRenderLayer extends BaseRenderLayer {
  private _state: ISelectionState;

  constructor(container: HTMLElement, zIndex: number, colors: IColorSet) {
    super(container, 'selection', zIndex, true, colors);
    this._clearState();
  }

  private _clearState(): void {
    this._state = {
      start: null,
      end: null,
      columnSelectMode: null,
      ydisp: null
    };
  }

  public resize(terminal: ITerminal, dim: IRenderDimensions): void {
    super.resize(terminal, dim);
    // Resizing the canvas discards the contents of the canvas so clear state
    this._clearState();
  }

  public reset(terminal: ITerminal): void {
    if (this._state.start && this._state.end) {
      this._clearState();
      this.clearAll();
    }
  }

  public onSelectionChanged(terminal: ITerminal, start: [number, number], end: [number, number], columnSelectMode: boolean): void {
    // Selection has not changed
    if (!this._didStateChange(start, end, columnSelectMode, terminal.buffer.ydisp)) {
      return;
    }

    // Remove all selections
    this.clearAll();

    // Selection does not exist
    if (!start || !end) {
      this._clearState();
      return;
    }

    // Translate from buffer position to viewport position
    const viewportStartRow = start[1] - terminal.buffer.ydisp;
    const viewportEndRow = end[1] - terminal.buffer.ydisp;
    const viewportCappedStartRow = Math.max(viewportStartRow, 0);
    const viewportCappedEndRow = Math.min(viewportEndRow, terminal.rows - 1);

    // No need to draw the selection
    if (viewportCappedStartRow >= terminal.rows || viewportCappedEndRow < 0) {
      return;
    }

    this._ctx.fillStyle = this._colors.selection.css;

    if (columnSelectMode) {
      const startCol = start[0];
      const width = end[0] - startCol;
      const height = viewportCappedEndRow - viewportCappedStartRow + 1;
      this.fillCells(startCol, viewportCappedStartRow, width, height);
    } else {
      // Draw first row
      const startCol = viewportStartRow === viewportCappedStartRow ? start[0] : 0;
      const startRowEndCol = viewportCappedStartRow === viewportCappedEndRow ? end[0] : terminal.cols;
      this.fillCells(startCol, viewportCappedStartRow, startRowEndCol - startCol, 1);

      // Draw middle rows
      const middleRowsCount = Math.max(viewportCappedEndRow - viewportCappedStartRow - 1, 0);
      this.fillCells(0, viewportCappedStartRow + 1, terminal.cols, middleRowsCount);

      // Draw final row
      if (viewportCappedStartRow !== viewportCappedEndRow) {
        // Only draw viewportEndRow if it's not the same as viewportStartRow
        const endCol = viewportEndRow === viewportCappedEndRow ? end[0] : terminal.cols;
        this.fillCells(0, viewportCappedEndRow, endCol, 1);
      }
    }

    // Save state for next render
    this._state.start = [start[0], start[1]];
    this._state.end = [end[0], end[1]];
    this._state.columnSelectMode = columnSelectMode;
    this._state.ydisp = terminal.buffer.ydisp;
  }

  private _didStateChange(start: [number, number], end: [number, number], columnSelectMode: boolean, ydisp: number): boolean {
    return !this._areCoordinatesEqual(start, this._state.start) ||
      !this._areCoordinatesEqual(end, this._state.end) ||
      columnSelectMode !== this._state.columnSelectMode ||
      ydisp !== this._state.ydisp;
  }

  private _areCoordinatesEqual(coord1: [number, number], coord2: [number, number]): boolean {
    if (!coord1 || !coord2) {
      return false;
    }

    return coord1[0] === coord2[0] && coord1[1] === coord2[1];
  }
}
