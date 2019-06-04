/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { ITerminal } from './Types';

/**
 * Represents a selection within the buffer. This model only cares about column
 * and row coordinates, not wide characters.
 */
export class SelectionModel {
  /**
   * Whether select all is currently active.
   */
  public isSelectAllActive: boolean;

  /**
   * The [x, y] position the selection starts at.
   */
  public selectionStart: [number, number];

  /**
   * The minimal length of the selection from the start position. When double
   * clicking on a word, the word will be selected which makes the selection
   * start at the start of the word and makes this variable the length.
   */
  public selectionStartLength: number;

  /**
   * The [x, y] position the selection ends at.
   */
  public selectionEnd: [number, number];

  constructor(
    private _terminal: ITerminal
  ) {
    this.clearSelection();
  }

  /**
   * Clears the current selection.
   */
  public clearSelection(): void {
    this.selectionStart = null;
    this.selectionEnd = null;
    this.isSelectAllActive = false;
    this.selectionStartLength = 0;
  }

  /**
   * The final selection start, taking into consideration select all.
   */
  public get finalSelectionStart(): [number, number] {
    if (this.isSelectAllActive) {
      return [0, 0];
    }

    if (!this.selectionEnd || !this.selectionStart) {
      return this.selectionStart;
    }

    return this.areSelectionValuesReversed() ? this.selectionEnd : this.selectionStart;
  }

  /**
   * The final selection end, taking into consideration select all, double click
   * word selection and triple click line selection.
   */
  public get finalSelectionEnd(): [number, number] {
    if (this.isSelectAllActive) {
      return [this._terminal.cols, this._terminal.buffer.ybase + this._terminal.rows - 1];
    }

    if (!this.selectionStart) {
      return null;
    }

    // Use the selection start + length if the end doesn't exist or they're reversed
    if (!this.selectionEnd || this.areSelectionValuesReversed()) {
      const startPlusLength = this.selectionStart[0] + this.selectionStartLength;
      if (startPlusLength > this._terminal.cols) {
        return [startPlusLength % this._terminal.cols, this.selectionStart[1] + Math.floor(startPlusLength / this._terminal.cols)];
      }
      return [startPlusLength, this.selectionStart[1]];
    }

    // Ensure the the word/line is selected after a double/triple click
    if (this.selectionStartLength) {
      // Select the larger of the two when start and end are on the same line
      if (this.selectionEnd[1] === this.selectionStart[1]) {
        return [Math.max(this.selectionStart[0] + this.selectionStartLength, this.selectionEnd[0]), this.selectionEnd[1]];
      }
    }
    return this.selectionEnd;
  }

  /**
   * Returns whether the selection start and end are reversed.
   */
  public areSelectionValuesReversed(): boolean {
    const start = this.selectionStart;
    const end = this.selectionEnd;
    if (!start || !end) {
      return false;
    }
    return start[1] > end[1] || (start[1] === end[1] && start[0] > end[0]);
  }

  /**
   * Handle the buffer being trimmed, adjust the selection position.
   * @param amount The amount the buffer is being trimmed.
   * @return Whether a refresh is necessary.
   */
  public onTrim(amount: number): boolean {
    // Adjust the selection position based on the trimmed amount.
    if (this.selectionStart) {
      this.selectionStart[1] -= amount;
    }
    if (this.selectionEnd) {
      this.selectionEnd[1] -= amount;
    }

    // The selection has moved off the buffer, clear it.
    if (this.selectionEnd && this.selectionEnd[1] < 0) {
      this.clearSelection();
      return true;
    }

    // If the selection start is trimmed, ensure the start column is 0.
    if (this.selectionStart && this.selectionStart[1] < 0) {
      this.selectionStart[1] = 0;
    }
    return false;
  }
}
