/**
 * Copyright (c) 2018 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { ITerminalOptions } from '../../Types';
import { IBufferLine } from '../../core/Types';
import { INVERTED_DEFAULT_COLOR } from '../atlas/Types';
import { CellData, AttributeData, NULL_CELL_CODE, WHITESPACE_CELL_CHAR } from '../../core/buffer/BufferLine';

export const BOLD_CLASS = 'xterm-bold';
export const DIM_CLASS = 'xterm-dim';
export const ITALIC_CLASS = 'xterm-italic';
export const UNDERLINE_CLASS = 'xterm-underline';
export const CURSOR_CLASS = 'xterm-cursor';
export const CURSOR_BLINK_CLASS = 'xterm-cursor-blink';
export const CURSOR_STYLE_BLOCK_CLASS = 'xterm-cursor-block';
export const CURSOR_STYLE_BAR_CLASS = 'xterm-cursor-bar';
export const CURSOR_STYLE_UNDERLINE_CLASS = 'xterm-cursor-underline';

export class DomRendererRowFactory {
  private _workCell: CellData = new CellData();

  constructor(
    private _terminalOptions: ITerminalOptions,
    private _document: Document
  ) {
  }

  public createRow(lineData: IBufferLine, isCursorRow: boolean, cursorStyle: string | undefined, cursorX: number, cursorBlink: boolean, cellWidth: number, cols: number): DocumentFragment {
    const fragment = this._document.createDocumentFragment();

    // Find the line length first, this prevents the need to output a bunch of
    // empty cells at the end. This cannot easily be integrated into the main
    // loop below because of the colCount feature (which can be removed after we
    // properly support reflow and disallow data to go beyond the right-side of
    // the viewport).
    let lineLength = 0;
    for (let x = Math.min(lineData.length, cols) - 1; x >= 0; x--) {
      if (lineData.loadCell(x, this._workCell).getCode() !== NULL_CELL_CODE || (isCursorRow && x === cursorX)) {
        lineLength = x + 1;
        break;
      }
    }

    for (let x = 0; x < lineLength; x++) {
      lineData.loadCell(x, this._workCell);
      const width = this._workCell.getWidth();

      // The character to the left is a wide character, drawing is owned by the char at x-1
      if (width === 0) {
        continue;
      }

      const charElement = this._document.createElement('span');
      if (width > 1) {
        charElement.style.width = `${cellWidth * width}px`;
      }

      if (isCursorRow && x === cursorX) {
        charElement.classList.add(CURSOR_CLASS);

        if (cursorBlink) {
          charElement.classList.add(CURSOR_BLINK_CLASS);
        }

        switch (cursorStyle) {
          case 'bar':
            charElement.classList.add(CURSOR_STYLE_BAR_CLASS);
            break;
          case 'underline':
            charElement.classList.add(CURSOR_STYLE_UNDERLINE_CLASS);
            break;
          default:
            charElement.classList.add(CURSOR_STYLE_BLOCK_CLASS);
            break;
        }
      }

      if (this._workCell.isBold() && this._terminalOptions.enableBold) {
        charElement.classList.add(BOLD_CLASS);
      }

      if (this._workCell.isItalic()) {
        charElement.classList.add(ITALIC_CLASS);
      }

      if (this._workCell.isDim()) {
        charElement.classList.add(DIM_CLASS);
      }

      if (this._workCell.isUnderline()) {
        charElement.classList.add(UNDERLINE_CLASS);
      }

      charElement.textContent = this._workCell.getChars() || WHITESPACE_CELL_CHAR;

      const swapColor = this._workCell.isInverse();

      // fg
      if (this._workCell.isFgRGB()) {
        let style = charElement.getAttribute('style') || '';
        style += `${swapColor ? 'background-' : ''}color:rgb(${(AttributeData.toColorRGB(this._workCell.getFgColor())).join(',')});`;
        charElement.setAttribute('style', style);
      } else if (this._workCell.isFgPalette()) {
        let fg = this._workCell.getFgColor();
        if (this._workCell.isBold() && fg < 8 && !swapColor &&
            this._terminalOptions.enableBold && this._terminalOptions.drawBoldTextInBrightColors) {
          fg += 8;
        }
        charElement.classList.add(`xterm-${swapColor ? 'b' : 'f'}g-${fg}`);
      } else if (swapColor) {
        charElement.classList.add(`xterm-bg-${INVERTED_DEFAULT_COLOR}`);
      }

      // bg
      if (this._workCell.isBgRGB()) {
        let style = charElement.getAttribute('style') || '';
        style += `${swapColor ? '' : 'background-'}color:rgb(${(AttributeData.toColorRGB(this._workCell.getBgColor())).join(',')});`;
        charElement.setAttribute('style', style);
      } else if (this._workCell.isBgPalette()) {
        charElement.classList.add(`xterm-${swapColor ? 'f' : 'b'}g-${this._workCell.getBgColor()}`);
      } else if (swapColor) {
        charElement.classList.add(`xterm-fg-${INVERTED_DEFAULT_COLOR}`);
      }

      fragment.appendChild(charElement);
    }
    return fragment;
  }
}
