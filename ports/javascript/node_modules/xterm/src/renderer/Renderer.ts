/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { TextRenderLayer } from './TextRenderLayer';
import { SelectionRenderLayer } from './SelectionRenderLayer';
import { CursorRenderLayer } from './CursorRenderLayer';
import { IRenderLayer, IRenderer, IRenderDimensions, ICharacterJoinerRegistry } from './Types';
import { ITerminal, CharacterJoinerHandler } from '../Types';
import { LinkRenderLayer } from './LinkRenderLayer';
import { CharacterJoinerRegistry } from '../renderer/CharacterJoinerRegistry';
import { Disposable } from '../common/Lifecycle';
import { IColorSet } from '../ui/Types';

export class Renderer extends Disposable implements IRenderer {
  private _renderLayers: IRenderLayer[];
  private _devicePixelRatio: number;
  private _characterJoinerRegistry: ICharacterJoinerRegistry;

  public dimensions: IRenderDimensions;

  constructor(
    private _terminal: ITerminal,
    private _colors: IColorSet
  ) {
    super();
    const allowTransparency = this._terminal.options.allowTransparency;
    this._characterJoinerRegistry = new CharacterJoinerRegistry(_terminal);

    this._renderLayers = [
      new TextRenderLayer(this._terminal.screenElement, 0, this._colors, this._characterJoinerRegistry, allowTransparency),
      new SelectionRenderLayer(this._terminal.screenElement, 1, this._colors),
      new LinkRenderLayer(this._terminal.screenElement, 2, this._colors, this._terminal),
      new CursorRenderLayer(this._terminal.screenElement, 3, this._colors)
    ];
    this.dimensions = {
      scaledCharWidth: null,
      scaledCharHeight: null,
      scaledCellWidth: null,
      scaledCellHeight: null,
      scaledCharLeft: null,
      scaledCharTop: null,
      scaledCanvasWidth: null,
      scaledCanvasHeight: null,
      canvasWidth: null,
      canvasHeight: null,
      actualCellWidth: null,
      actualCellHeight: null
    };
    this._devicePixelRatio = window.devicePixelRatio;
    this._updateDimensions();
    this.onOptionsChanged();
  }

  public dispose(): void {
    super.dispose();
    this._renderLayers.forEach(l => l.dispose());
  }

  public onDevicePixelRatioChange(): void {
    // If the device pixel ratio changed, the char atlas needs to be regenerated
    // and the terminal needs to refreshed
    if (this._devicePixelRatio !== window.devicePixelRatio) {
      this._devicePixelRatio = window.devicePixelRatio;
      this.onResize(this._terminal.cols, this._terminal.rows);
    }
  }

  public setColors(colors: IColorSet): void {
    this._colors = colors;

    // Clear layers and force a full render
    this._renderLayers.forEach(l => {
      l.setColors(this._terminal, this._colors);
      l.reset(this._terminal);
    });
  }

  public onResize(cols: number, rows: number): void {
    // Update character and canvas dimensions
    this._updateDimensions();

    // Resize all render layers
    this._renderLayers.forEach(l => l.resize(this._terminal, this.dimensions));

    // Resize the screen
    this._terminal.screenElement.style.width = `${this.dimensions.canvasWidth}px`;
    this._terminal.screenElement.style.height = `${this.dimensions.canvasHeight}px`;
  }

  public onCharSizeChanged(): void {
    this.onResize(this._terminal.cols, this._terminal.rows);
  }

  public onBlur(): void {
    this._runOperation(l => l.onBlur(this._terminal));
  }

  public onFocus(): void {
    this._runOperation(l => l.onFocus(this._terminal));
  }

  public onSelectionChanged(start: [number, number], end: [number, number], columnSelectMode: boolean = false): void {
    this._runOperation(l => l.onSelectionChanged(this._terminal, start, end, columnSelectMode));
  }

  public onCursorMove(): void {
    this._runOperation(l => l.onCursorMove(this._terminal));
  }

  public onOptionsChanged(): void {
    this._runOperation(l => l.onOptionsChanged(this._terminal));
  }

  public clear(): void {
    this._runOperation(l => l.reset(this._terminal));
  }

  private _runOperation(operation: (layer: IRenderLayer) => void): void {
    this._renderLayers.forEach(l => operation(l));
  }

  /**
   * Performs the refresh loop callback, calling refresh only if a refresh is
   * necessary before queueing up the next one.
   */
  public renderRows(start: number, end: number): void {
    this._renderLayers.forEach(l => l.onGridChanged(this._terminal, start, end));
  }

  /**
   * Recalculates the character and canvas dimensions.
   */
  private _updateDimensions(): void {
    // Perform a new measure if the CharMeasure dimensions are not yet available
    if (!this._terminal.charMeasure.width || !this._terminal.charMeasure.height) {
      return;
    }

    // Calculate the scaled character width. Width is floored as it must be
    // drawn to an integer grid in order for the CharAtlas "stamps" to not be
    // blurry. When text is drawn to the grid not using the CharAtlas, it is
    // clipped to ensure there is no overlap with the next cell.
    this.dimensions.scaledCharWidth = Math.floor(this._terminal.charMeasure.width * window.devicePixelRatio);

    // Calculate the scaled character height. Height is ceiled in case
    // devicePixelRatio is a floating point number in order to ensure there is
    // enough space to draw the character to the cell.
    this.dimensions.scaledCharHeight = Math.ceil(this._terminal.charMeasure.height * window.devicePixelRatio);

    // Calculate the scaled cell height, if lineHeight is not 1 then the value
    // will be floored because since lineHeight can never be lower then 1, there
    // is a guarentee that the scaled line height will always be larger than
    // scaled char height.
    this.dimensions.scaledCellHeight = Math.floor(this.dimensions.scaledCharHeight * this._terminal.options.lineHeight);

    // Calculate the y coordinate within a cell that text should draw from in
    // order to draw in the center of a cell.
    this.dimensions.scaledCharTop = this._terminal.options.lineHeight === 1 ? 0 : Math.round((this.dimensions.scaledCellHeight - this.dimensions.scaledCharHeight) / 2);

    // Calculate the scaled cell width, taking the letterSpacing into account.
    this.dimensions.scaledCellWidth = this.dimensions.scaledCharWidth + Math.round(this._terminal.options.letterSpacing);

    // Calculate the x coordinate with a cell that text should draw from in
    // order to draw in the center of a cell.
    this.dimensions.scaledCharLeft = Math.floor(this._terminal.options.letterSpacing / 2);

    // Recalculate the canvas dimensions; scaled* define the actual number of
    // pixel in the canvas
    this.dimensions.scaledCanvasHeight = this._terminal.rows * this.dimensions.scaledCellHeight;
    this.dimensions.scaledCanvasWidth = this._terminal.cols * this.dimensions.scaledCellWidth;

    // The the size of the canvas on the page. It's very important that this
    // rounds to nearest integer and not ceils as browsers often set
    // window.devicePixelRatio as something like 1.100000023841858, when it's
    // actually 1.1. Ceiling causes blurriness as the backing canvas image is 1
    // pixel too large for the canvas element size.
    this.dimensions.canvasHeight = Math.round(this.dimensions.scaledCanvasHeight / window.devicePixelRatio);
    this.dimensions.canvasWidth = Math.round(this.dimensions.scaledCanvasWidth / window.devicePixelRatio);

    // Get the _actual_ dimensions of an individual cell. This needs to be
    // derived from the canvasWidth/Height calculated above which takes into
    // account window.devicePixelRatio. CharMeasure.width/height by itself is
    // insufficient when the page is not at 100% zoom level as CharMeasure is
    // measured in CSS pixels, but the actual char size on the canvas can
    // differ.
    this.dimensions.actualCellHeight = this.dimensions.canvasHeight / this._terminal.rows;
    this.dimensions.actualCellWidth = this.dimensions.canvasWidth / this._terminal.cols;
  }

  public registerCharacterJoiner(handler: CharacterJoinerHandler): number {
    return this._characterJoinerRegistry.registerCharacterJoiner(handler);
  }

  public deregisterCharacterJoiner(joinerId: number): boolean {
    return this._characterJoinerRegistry.deregisterCharacterJoiner(joinerId);
  }
}
