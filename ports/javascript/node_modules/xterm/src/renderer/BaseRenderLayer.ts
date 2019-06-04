/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { IRenderLayer, IRenderDimensions } from './Types';
import { ITerminal } from '../Types';
import { ICellData } from '../core/Types';
import { DEFAULT_COLOR } from '../common/Types';
import { DIM_OPACITY, INVERTED_DEFAULT_COLOR, IGlyphIdentifier } from './atlas/Types';
import BaseCharAtlas from './atlas/BaseCharAtlas';
import { acquireCharAtlas } from './atlas/CharAtlasCache';
import { CellData, AttributeData, WHITESPACE_CELL_CHAR, WHITESPACE_CELL_CODE } from '../core/buffer/BufferLine';
import { IColorSet } from '../ui/Types';

export abstract class BaseRenderLayer implements IRenderLayer {
  private _canvas: HTMLCanvasElement;
  protected _ctx: CanvasRenderingContext2D;
  private _scaledCharWidth: number = 0;
  private _scaledCharHeight: number = 0;
  private _scaledCellWidth: number = 0;
  private _scaledCellHeight: number = 0;
  private _scaledCharLeft: number = 0;
  private _scaledCharTop: number = 0;

  protected _charAtlas: BaseCharAtlas;

  /**
   * An object that's reused when drawing glyphs in order to reduce GC.
   */
  private _currentGlyphIdentifier: IGlyphIdentifier = {
    chars: '',
    code: 0,
    bg: 0,
    fg: 0,
    bold: false,
    dim: false,
    italic: false
  };

  constructor(
    private _container: HTMLElement,
    id: string,
    zIndex: number,
    private _alpha: boolean,
    protected _colors: IColorSet
  ) {
    this._canvas = document.createElement('canvas');
    this._canvas.classList.add(`xterm-${id}-layer`);
    this._canvas.style.zIndex = zIndex.toString();
    this._initCanvas();
    this._container.appendChild(this._canvas);
  }

  public dispose(): void {
    this._container.removeChild(this._canvas);
    if (this._charAtlas) {
      this._charAtlas.dispose();
    }
  }

  private _initCanvas(): void {
    this._ctx = this._canvas.getContext('2d', {alpha: this._alpha});
    // Draw the background if this is an opaque layer
    if (!this._alpha) {
      this.clearAll();
    }
  }

  public onOptionsChanged(terminal: ITerminal): void {}
  public onBlur(terminal: ITerminal): void {}
  public onFocus(terminal: ITerminal): void {}
  public onCursorMove(terminal: ITerminal): void {}
  public onGridChanged(terminal: ITerminal, startRow: number, endRow: number): void {}
  public onSelectionChanged(terminal: ITerminal, start: [number, number], end: [number, number], columnSelectMode: boolean = false): void {}

  public setColors(terminal: ITerminal, colorSet: IColorSet): void {
    this._refreshCharAtlas(terminal, colorSet);
  }

  protected setTransparency(terminal: ITerminal, alpha: boolean): void {
    // Do nothing when alpha doesn't change
    if (alpha === this._alpha) {
      return;
    }

    // Create new canvas and replace old one
    const oldCanvas = this._canvas;
    this._alpha = alpha;
    // Cloning preserves properties
    this._canvas = <HTMLCanvasElement>this._canvas.cloneNode();
    this._initCanvas();
    this._container.replaceChild(this._canvas, oldCanvas);

    // Regenerate char atlas and force a full redraw
    this._refreshCharAtlas(terminal, this._colors);
    this.onGridChanged(terminal, 0, terminal.rows - 1);
  }

  /**
   * Refreshes the char atlas, aquiring a new one if necessary.
   * @param terminal The terminal.
   * @param colorSet The color set to use for the char atlas.
   */
  private _refreshCharAtlas(terminal: ITerminal, colorSet: IColorSet): void {
    if (this._scaledCharWidth <= 0 && this._scaledCharHeight <= 0) {
      return;
    }
    this._charAtlas = acquireCharAtlas(terminal, colorSet, this._scaledCharWidth, this._scaledCharHeight);
    this._charAtlas.warmUp();
  }

  public resize(terminal: ITerminal, dim: IRenderDimensions): void {
    this._scaledCellWidth = dim.scaledCellWidth;
    this._scaledCellHeight = dim.scaledCellHeight;
    this._scaledCharWidth = dim.scaledCharWidth;
    this._scaledCharHeight = dim.scaledCharHeight;
    this._scaledCharLeft = dim.scaledCharLeft;
    this._scaledCharTop = dim.scaledCharTop;
    this._canvas.width = dim.scaledCanvasWidth;
    this._canvas.height = dim.scaledCanvasHeight;
    this._canvas.style.width = `${dim.canvasWidth}px`;
    this._canvas.style.height = `${dim.canvasHeight}px`;

    // Draw the background if this is an opaque layer
    if (!this._alpha) {
      this.clearAll();
    }

    this._refreshCharAtlas(terminal, this._colors);
  }

  public abstract reset(terminal: ITerminal): void;

  /**
   * Fills 1+ cells completely. This uses the existing fillStyle on the context.
   * @param x The column to start at.
   * @param y The row to start at
   * @param width The number of columns to fill.
   * @param height The number of rows to fill.
   */
  protected fillCells(x: number, y: number, width: number, height: number): void {
    this._ctx.fillRect(
        x * this._scaledCellWidth,
        y * this._scaledCellHeight,
        width * this._scaledCellWidth,
        height * this._scaledCellHeight);
  }

  /**
   * Fills a 1px line (2px on HDPI) at the bottom of the cell. This uses the
   * existing fillStyle on the context.
   * @param x The column to fill.
   * @param y The row to fill.
   */
  protected fillBottomLineAtCells(x: number, y: number, width: number = 1): void {
    this._ctx.fillRect(
        x * this._scaledCellWidth,
        (y + 1) * this._scaledCellHeight - window.devicePixelRatio - 1 /* Ensure it's drawn within the cell */,
        width * this._scaledCellWidth,
        window.devicePixelRatio);
  }

  /**
   * Fills a 1px line (2px on HDPI) at the left of the cell. This uses the
   * existing fillStyle on the context.
   * @param x The column to fill.
   * @param y The row to fill.
   */
  protected fillLeftLineAtCell(x: number, y: number): void {
    this._ctx.fillRect(
        x * this._scaledCellWidth,
        y * this._scaledCellHeight,
        window.devicePixelRatio,
        this._scaledCellHeight);
  }

  /**
   * Strokes a 1px rectangle (2px on HDPI) around a cell. This uses the existing
   * strokeStyle on the context.
   * @param x The column to fill.
   * @param y The row to fill.
   */
  protected strokeRectAtCell(x: number, y: number, width: number, height: number): void {
    this._ctx.lineWidth = window.devicePixelRatio;
    this._ctx.strokeRect(
        x * this._scaledCellWidth + window.devicePixelRatio / 2,
        y * this._scaledCellHeight + (window.devicePixelRatio / 2),
        width * this._scaledCellWidth - window.devicePixelRatio,
        (height * this._scaledCellHeight) - window.devicePixelRatio);
  }

  /**
   * Clears the entire canvas.
   */
  protected clearAll(): void {
    if (this._alpha) {
      this._ctx.clearRect(0, 0, this._canvas.width, this._canvas.height);
    } else {
      this._ctx.fillStyle = this._colors.background.css;
      this._ctx.fillRect(0, 0, this._canvas.width, this._canvas.height);
    }
  }

  /**
   * Clears 1+ cells completely.
   * @param x The column to start at.
   * @param y The row to start at.
   * @param width The number of columns to clear.
   * @param height The number of rows to clear.
   */
  protected clearCells(x: number, y: number, width: number, height: number): void {
    if (this._alpha) {
      this._ctx.clearRect(
          x * this._scaledCellWidth,
          y * this._scaledCellHeight,
          width * this._scaledCellWidth,
          height * this._scaledCellHeight);
    } else {
      this._ctx.fillStyle = this._colors.background.css;
      this._ctx.fillRect(
          x * this._scaledCellWidth,
          y * this._scaledCellHeight,
          width * this._scaledCellWidth,
          height * this._scaledCellHeight);
    }
  }

  /**
   * Draws a truecolor character at the cell. The character will be clipped to
   * ensure that it fits with the cell, including the cell to the right if it's
   * a wide character. This uses the existing fillStyle on the context.
   * @param terminal The terminal.
   * @param cell The cell data for the character to draw.
   * @param x The column to draw at.
   * @param y The row to draw at.
   * @param color The color of the character.
   */
  protected fillCharTrueColor(terminal: ITerminal, cell: CellData, x: number, y: number): void {
    this._ctx.font = this._getFont(terminal, false, false);
    this._ctx.textBaseline = 'middle';
    this._clipRow(terminal, y);
    this._ctx.fillText(
        cell.getChars(),
        x * this._scaledCellWidth + this._scaledCharLeft,
        y * this._scaledCellHeight + this._scaledCharTop + this._scaledCharHeight / 2);
  }

  /**
   * Draws one or more characters at a cell. If possible this will draw using
   * the character atlas to reduce draw time.
   * @param terminal The terminal.
   * @param chars The character or characters.
   * @param code The character code.
   * @param width The width of the characters.
   * @param x The column to draw at.
   * @param y The row to draw at.
   * @param fg The foreground color, in the format stored within the attributes.
   * @param bg The background color, in the format stored within the attributes.
   * This is used to validate whether a cached image can be used.
   * @param bold Whether the text is bold.
   */
  protected drawChars(terminal: ITerminal, cell: ICellData, x: number, y: number): void {

    // skip cache right away if we draw in RGB
    // Note: to avoid bad runtime JoinedCellData will be skipped
    //       in the cache handler itself (atlasDidDraw == false) and
    //       fall through to uncached later down below
    if (cell.isFgRGB() || cell.isBgRGB()) {
      this._drawUncachedChars(terminal, cell, x, y);
      return;
    }

    let fg;
    let bg;
    if (cell.isInverse()) {
      fg = (cell.isBgDefault()) ? INVERTED_DEFAULT_COLOR : cell.getBgColor();
      bg = (cell.isFgDefault()) ? INVERTED_DEFAULT_COLOR : cell.getFgColor();
    } else {
      bg = (cell.isBgDefault()) ? DEFAULT_COLOR : cell.getBgColor();
      fg = (cell.isFgDefault()) ? DEFAULT_COLOR : cell.getFgColor();
    }

    const drawInBrightColor = terminal.options.drawBoldTextInBrightColors && cell.isBold() && fg < 8 && fg !== INVERTED_DEFAULT_COLOR;

    fg += drawInBrightColor ? 8 : 0;
    this._currentGlyphIdentifier.chars = cell.getChars() || WHITESPACE_CELL_CHAR;
    this._currentGlyphIdentifier.code = cell.getCode() || WHITESPACE_CELL_CODE;
    this._currentGlyphIdentifier.bg = bg;
    this._currentGlyphIdentifier.fg = fg;
    this._currentGlyphIdentifier.bold = cell.isBold() && terminal.options.enableBold;
    this._currentGlyphIdentifier.dim = !!cell.isDim();
    this._currentGlyphIdentifier.italic = !!cell.isItalic();
    const atlasDidDraw = this._charAtlas && this._charAtlas.draw(
      this._ctx,
      this._currentGlyphIdentifier,
      x * this._scaledCellWidth + this._scaledCharLeft,
      y * this._scaledCellHeight + this._scaledCharTop
    );

    if (!atlasDidDraw) {
      this._drawUncachedChars(terminal, cell, x, y);
    }
  }

  /**
   * Draws one or more characters at one or more cells. The character(s) will be
   * clipped to ensure that they fit with the cell(s), including the cell to the
   * right if the last character is a wide character.
   * @param terminal The terminal.
   * @param chars The character.
   * @param width The width of the character.
   * @param fg The foreground color, in the format stored within the attributes.
   * @param x The column to draw at.
   * @param y The row to draw at.
   */
  private _drawUncachedChars(terminal: ITerminal, cell: ICellData, x: number, y: number): void {
    this._ctx.save();
    this._ctx.font = this._getFont(terminal, cell.isBold() && terminal.options.enableBold, !!cell.isItalic());
    this._ctx.textBaseline = 'middle';

    if (cell.isInverse()) {
      if (cell.isBgDefault()) {
        this._ctx.fillStyle = this._colors.background.css;
      } else if (cell.isBgRGB()) {
        this._ctx.fillStyle = `rgb(${AttributeData.toColorRGB(cell.getBgColor()).join(',')})`;
      } else {
        this._ctx.fillStyle = this._colors.ansi[cell.getBgColor()].css;
      }
    } else {
      if (cell.isFgDefault()) {
        this._ctx.fillStyle = this._colors.foreground.css;
      } else if (cell.isFgRGB()) {
        this._ctx.fillStyle = `rgb(${AttributeData.toColorRGB(cell.getFgColor()).join(',')})`;
      } else {
        let fg = cell.getFgColor();
        if (terminal.options.drawBoldTextInBrightColors && cell.isBold() && fg < 8) {
          fg += 8;
        }
        this._ctx.fillStyle = this._colors.ansi[fg].css;
      }
    }

    this._clipRow(terminal, y);

    // Apply alpha to dim the character
    if (cell.isDim()) {
      this._ctx.globalAlpha = DIM_OPACITY;
    }
    // Draw the character
    this._ctx.fillText(
        cell.getChars(),
        x * this._scaledCellWidth + this._scaledCharLeft,
        y * this._scaledCellHeight + this._scaledCharTop + this._scaledCharHeight / 2);
    this._ctx.restore();
  }

  /**
   * Clips a row to ensure no pixels will be drawn outside the cells in the row.
   * @param terminal The terminal.
   * @param y The row to clip.
   */
  private _clipRow(terminal: ITerminal, y: number): void {
    this._ctx.beginPath();
    this._ctx.rect(
        0,
        y * this._scaledCellHeight,
        terminal.cols * this._scaledCellWidth,
        this._scaledCellHeight);
    this._ctx.clip();
  }

  /**
   * Gets the current font.
   * @param terminal The terminal.
   * @param isBold If we should use the bold fontWeight.
   */
  protected _getFont(terminal: ITerminal, isBold: boolean, isItalic: boolean): string {
    const fontWeight = isBold ? terminal.options.fontWeightBold : terminal.options.fontWeight;
    const fontStyle = isItalic ? 'italic' : '';

    return `${fontStyle} ${fontWeight} ${terminal.options.fontSize * window.devicePixelRatio}px ${terminal.options.fontFamily}`;
  }
}

