/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import * as Strings from './Strings';
import { ITerminal, IBuffer } from './Types';
import { isMac } from './common/Platform';
import { RenderDebouncer } from './ui/RenderDebouncer';
import { addDisposableDomListener } from './ui/Lifecycle';
import { Disposable } from './common/Lifecycle';
import { ScreenDprMonitor } from './ui/ScreenDprMonitor';
import { IRenderDimensions } from './renderer/Types';

const MAX_ROWS_TO_READ = 20;

const enum BoundaryPosition {
  TOP,
  BOTTOM
}

export class AccessibilityManager extends Disposable {
  private _accessibilityTreeRoot: HTMLElement;
  private _rowContainer: HTMLElement;
  private _rowElements: HTMLElement[];
  private _liveRegion: HTMLElement;
  private _liveRegionLineCount: number = 0;

  private _renderRowsDebouncer: RenderDebouncer;
  private _screenDprMonitor: ScreenDprMonitor;

  private _topBoundaryFocusListener: (e: FocusEvent) => void;
  private _bottomBoundaryFocusListener: (e: FocusEvent) => void;

  /**
   * This queue has a character pushed to it for keys that are pressed, if the
   * next character added to the terminal is equal to the key char then it is
   * not announced (added to live region) because it has already been announced
   * by the textarea event (which cannot be canceled). There are some race
   * condition cases if there is typing while data is streaming, but this covers
   * the main case of typing into the prompt and inputting the answer to a
   * question (Y/N, etc.).
   */
  private _charsToConsume: string[] = [];

  private _charsToAnnounce: string = '';

  constructor(
    private _terminal: ITerminal,
    private _dimensions: IRenderDimensions
  ) {
    super();
    this._accessibilityTreeRoot = document.createElement('div');
    this._accessibilityTreeRoot.classList.add('xterm-accessibility');

    this._rowContainer = document.createElement('div');
    this._rowContainer.classList.add('xterm-accessibility-tree');
    this._rowElements = [];
    for (let i = 0; i < this._terminal.rows; i++) {
      this._rowElements[i] = this._createAccessibilityTreeNode();
      this._rowContainer.appendChild(this._rowElements[i]);
    }

    this._topBoundaryFocusListener = e => this._onBoundaryFocus(e, BoundaryPosition.TOP);
    this._bottomBoundaryFocusListener = e => this._onBoundaryFocus(e, BoundaryPosition.BOTTOM);
    this._rowElements[0].addEventListener('focus', this._topBoundaryFocusListener);
    this._rowElements[this._rowElements.length - 1].addEventListener('focus', this._bottomBoundaryFocusListener);

    this._refreshRowsDimensions();
    this._accessibilityTreeRoot.appendChild(this._rowContainer);

    this._renderRowsDebouncer = new RenderDebouncer(this._renderRows.bind(this));
    this._refreshRows();

    this._liveRegion = document.createElement('div');
    this._liveRegion.classList.add('live-region');
    this._liveRegion.setAttribute('aria-live', 'assertive');
    this._accessibilityTreeRoot.appendChild(this._liveRegion);

    this._terminal.element.insertAdjacentElement('afterbegin', this._accessibilityTreeRoot);

    this.register(this._renderRowsDebouncer);
    this.register(this._terminal.onResize(e => this._onResize(e.rows)));
    this.register(this._terminal.onRender(e => this._refreshRows(e.start, e.end)));
    this.register(this._terminal.onScroll(() => this._refreshRows()));
    // Line feed is an issue as the prompt won't be read out after a command is run
    this.register(this._terminal.addDisposableListener('a11y.char', (char) => this._onChar(char)));
    this.register(this._terminal.onLineFeed(() => this._onChar('\n')));
    this.register(this._terminal.addDisposableListener('a11y.tab', spaceCount => this._onTab(spaceCount)));
    this.register(this._terminal.onKey(e => this._onKey(e.key)));
    this.register(this._terminal.addDisposableListener('blur', () => this._clearLiveRegion()));

    this._screenDprMonitor = new ScreenDprMonitor();
    this.register(this._screenDprMonitor);
    this._screenDprMonitor.setListener(() => this._refreshRowsDimensions());
    // This shouldn't be needed on modern browsers but is present in case the
    // media query that drives the ScreenDprMonitor isn't supported
    this.register(addDisposableDomListener(window, 'resize', () => this._refreshRowsDimensions()));
  }

  public dispose(): void {
    super.dispose();
    this._terminal.element.removeChild(this._accessibilityTreeRoot);
    this._rowElements.length = 0;
  }

  private _onBoundaryFocus(e: FocusEvent, position: BoundaryPosition): void {
    const boundaryElement = <HTMLElement>e.target;
    const beforeBoundaryElement = this._rowElements[position === BoundaryPosition.TOP ? 1 : this._rowElements.length - 2];

    // Don't scroll if the buffer top has reached the end in that direction
    const posInSet = boundaryElement.getAttribute('aria-posinset');
    const lastRowPos = position === BoundaryPosition.TOP ? '1' : `${this._terminal.buffer.lines.length}`;
    if (posInSet === lastRowPos) {
      return;
    }

    // Don't scroll when the last focused item was not the second row (focus is going the other
    // direction)
    if (e.relatedTarget !== beforeBoundaryElement) {
      return;
    }

    // Remove old boundary element from array
    let topBoundaryElement: HTMLElement;
    let bottomBoundaryElement: HTMLElement;
    if (position === BoundaryPosition.TOP) {
      topBoundaryElement = boundaryElement;
      bottomBoundaryElement = this._rowElements.pop()!;
      this._rowContainer.removeChild(bottomBoundaryElement);
    } else {
      topBoundaryElement = this._rowElements.shift()!;
      bottomBoundaryElement = boundaryElement;
      this._rowContainer.removeChild(topBoundaryElement);
    }

    // Remove listeners from old boundary elements
    topBoundaryElement.removeEventListener('focus', this._topBoundaryFocusListener);
    bottomBoundaryElement.removeEventListener('focus', this._bottomBoundaryFocusListener);

    // Add new element to array/DOM
    if (position === BoundaryPosition.TOP) {
      const newElement = this._createAccessibilityTreeNode();
      this._rowElements.unshift(newElement);
      this._rowContainer.insertAdjacentElement('afterbegin', newElement);
    } else {
      const newElement = this._createAccessibilityTreeNode();
      this._rowElements.push(newElement);
      this._rowContainer.appendChild(newElement);
    }

    // Add listeners to new boundary elements
    this._rowElements[0].addEventListener('focus', this._topBoundaryFocusListener);
    this._rowElements[this._rowElements.length - 1].addEventListener('focus', this._bottomBoundaryFocusListener);

    // Scroll up
    this._terminal.scrollLines(position === BoundaryPosition.TOP ? -1 : 1);

    // Focus new boundary before element
    this._rowElements[position === BoundaryPosition.TOP ? 1 : this._rowElements.length - 2].focus();

    // Prevent the standard behavior
    e.preventDefault();
    e.stopImmediatePropagation();
  }

  private _onResize(rows: number): void {
    // Remove bottom boundary listener
    this._rowElements[this._rowElements.length - 1].removeEventListener('focus', this._bottomBoundaryFocusListener);

    // Grow rows as required
    for (let i = this._rowContainer.children.length; i < this._terminal.rows; i++) {
      this._rowElements[i] = this._createAccessibilityTreeNode();
      this._rowContainer.appendChild(this._rowElements[i]);
    }
    // Shrink rows as required
    while (this._rowElements.length > rows) {
      this._rowContainer.removeChild(this._rowElements.pop()!);
    }

    // Add bottom boundary listener
    this._rowElements[this._rowElements.length - 1].addEventListener('focus', this._bottomBoundaryFocusListener);

    this._refreshRowsDimensions();
  }

  private _createAccessibilityTreeNode(): HTMLElement {
    const element = document.createElement('div');
    element.setAttribute('role', 'listitem');
    element.tabIndex = -1;
    this._refreshRowDimensions(element);
    return element;
  }

  private _onTab(spaceCount: number): void {
    for (let i = 0; i < spaceCount; i++) {
      this._onChar(' ');
    }
  }

  private _onChar(char: string): void {
    if (this._liveRegionLineCount < MAX_ROWS_TO_READ + 1) {
      if (this._charsToConsume.length > 0) {
        // Have the screen reader ignore the char if it was just input
        const shiftedChar = this._charsToConsume.shift();
        if (shiftedChar !== char) {
          this._charsToAnnounce += char;
        }
      } else {
        this._charsToAnnounce += char;
      }

      if (char === '\n') {
        this._liveRegionLineCount++;
        if (this._liveRegionLineCount === MAX_ROWS_TO_READ + 1) {
          this._liveRegion.textContent += Strings.tooMuchOutput;
        }
      }

      // Only detach/attach on mac as otherwise messages can go unaccounced
      if (isMac) {
        if (this._liveRegion.textContent && this._liveRegion.textContent.length > 0 && !this._liveRegion.parentNode) {
          setTimeout(() => {
            this._accessibilityTreeRoot.appendChild(this._liveRegion);
          }, 0);
        }
      }
    }
  }

  private _clearLiveRegion(): void {
    this._liveRegion.textContent = '';
    this._liveRegionLineCount = 0;

    // Only detach/attach on mac as otherwise messages can go unaccounced
    if (isMac) {
      if (this._liveRegion.parentNode) {
        this._accessibilityTreeRoot.removeChild(this._liveRegion);
      }
    }
  }

  private _onKey(keyChar: string): void {
    this._clearLiveRegion();
    this._charsToConsume.push(keyChar);
  }

  private _refreshRows(start?: number, end?: number): void {
    this._renderRowsDebouncer.refresh(start, end, this._terminal.rows);
  }

  private _renderRows(start: number, end: number): void {
    const buffer: IBuffer = this._terminal.buffer;
    const setSize = buffer.lines.length.toString();
    for (let i = start; i <= end; i++) {
      const lineData = buffer.translateBufferLineToString(buffer.ydisp + i, true);
      const posInSet = (buffer.ydisp + i + 1).toString();
      const element = this._rowElements[i];
      if (element) {
        element.textContent = lineData.length === 0 ? Strings.blankLine : lineData;
        element.setAttribute('aria-posinset', posInSet);
        element.setAttribute('aria-setsize', setSize);
      }
    }
    this._announceCharacters();
  }

  private _refreshRowsDimensions(): void {
    if (!this._dimensions.actualCellHeight) {
      return;
    }
    if (this._rowElements.length !== this._terminal.rows) {
      this._onResize(this._terminal.rows);
    }
    for (let i = 0; i < this._terminal.rows; i++) {
      this._refreshRowDimensions(this._rowElements[i]);
    }
  }

  public setDimensions(dimensions: IRenderDimensions): void {
    this._dimensions = dimensions;
    this._refreshRowsDimensions();
  }

  private _refreshRowDimensions(element: HTMLElement): void {
    element.style.height = `${this._dimensions.actualCellHeight}px`;
  }

  private _announceCharacters(): void {
    if (this._charsToAnnounce.length === 0) {
      return;
    }
    this._liveRegion.textContent += this._charsToAnnounce;
    this._charsToAnnounce = '';
  }
}
