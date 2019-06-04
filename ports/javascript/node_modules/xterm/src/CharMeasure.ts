/**
 * Copyright (c) 2016 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { ICharMeasure, ITerminalOptions } from './Types';
import { EventEmitter2, IEvent } from './common/EventEmitter2';

/**
 * Utility class that measures the size of a character. Measurements are done in
 * the DOM rather than with a canvas context because support for extracting the
 * height of characters is patchy across browsers.
 */
export class CharMeasure implements ICharMeasure {
  private _document: Document;
  private _parentElement: HTMLElement;
  private _measureElement: HTMLElement;
  private _width: number;
  private _height: number;

  private _onCharSizeChanged = new EventEmitter2<void>();
  public get onCharSizeChanged(): IEvent<void> { return this._onCharSizeChanged.event; }

  constructor(document: Document, parentElement: HTMLElement) {
    this._document = document;
    this._parentElement = parentElement;
    this._measureElement = this._document.createElement('span');
    this._measureElement.classList.add('xterm-char-measure-element');
    this._measureElement.textContent = 'W';
    this._measureElement.setAttribute('aria-hidden', 'true');
    this._parentElement.appendChild(this._measureElement);
  }

  public get width(): number {
    return this._width;
  }

  public get height(): number {
    return this._height;
  }

  public measure(options: ITerminalOptions): void {
    this._measureElement.style.fontFamily = options.fontFamily;
    this._measureElement.style.fontSize = `${options.fontSize}px`;
    const geometry = this._measureElement.getBoundingClientRect();
    // The element is likely currently display:none, we should retain the
    // previous value.
    if (geometry.width === 0 || geometry.height === 0) {
      return;
    }
    const adjustedHeight = Math.ceil(geometry.height);
    if (this._width !== geometry.width || this._height !== adjustedHeight) {
      this._width = geometry.width;
      this._height = adjustedHeight;
      this._onCharSizeChanged.fire();
    }
  }
}
