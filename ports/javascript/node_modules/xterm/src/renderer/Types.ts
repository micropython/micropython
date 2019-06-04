/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { ITerminal, CharacterJoinerHandler } from '../Types';
import { IDisposable } from 'xterm';
import { IColorSet } from '../ui/Types';

/**
 * Flags used to render terminal text properly.
 */
export const enum FLAGS {
  BOLD = 1,
  UNDERLINE = 2,
  BLINK = 4,
  INVERSE = 8,
  INVISIBLE = 16,
  DIM = 32,
  ITALIC = 64
}

/**
 * Note that IRenderer implementations should emit the refresh event after
 * rendering rows to the screen.
 */
export interface IRenderer extends IDisposable {
  readonly dimensions: IRenderDimensions;

  dispose(): void;
  setColors(colors: IColorSet): void;
  onDevicePixelRatioChange(): void;
  onResize(cols: number, rows: number): void;
  onCharSizeChanged(): void;
  onBlur(): void;
  onFocus(): void;
  onSelectionChanged(start: [number, number], end: [number, number], columnSelectMode: boolean): void;
  onCursorMove(): void;
  onOptionsChanged(): void;
  clear(): void;
  renderRows(start: number, end: number): void;
  registerCharacterJoiner(handler: CharacterJoinerHandler): number;
  deregisterCharacterJoiner(joinerId: number): boolean;
}

export interface IRenderDimensions {
  scaledCharWidth: number;
  scaledCharHeight: number;
  scaledCellWidth: number;
  scaledCellHeight: number;
  scaledCharLeft: number;
  scaledCharTop: number;
  scaledCanvasWidth: number;
  scaledCanvasHeight: number;
  canvasWidth: number;
  canvasHeight: number;
  actualCellWidth: number;
  actualCellHeight: number;
}

export interface IRenderLayer extends IDisposable {
  /**
   * Called when the terminal loses focus.
   */
  onBlur(terminal: ITerminal): void;

  /**
   * * Called when the terminal gets focus.
   */
  onFocus(terminal: ITerminal): void;

  /**
   * Called when the cursor is moved.
   */
  onCursorMove(terminal: ITerminal): void;

  /**
   * Called when options change.
   */
  onOptionsChanged(terminal: ITerminal): void;

  /**
   * Called when the theme changes.
   */
  setColors(terminal: ITerminal, colorSet: IColorSet): void;

  /**
   * Called when the data in the grid has changed (or needs to be rendered
   * again).
   */
  onGridChanged(terminal: ITerminal, startRow: number, endRow: number): void;

  /**
   * Calls when the selection changes.
   */
  onSelectionChanged(terminal: ITerminal, start: [number, number], end: [number, number], columnSelectMode: boolean): void;

  /**
   * Registers a handler to join characters to render as a group
   */
  registerCharacterJoiner?(joiner: ICharacterJoiner): void;

  /**
   * Deregisters the specified character joiner handler
   */
  deregisterCharacterJoiner?(joinerId: number): void;

  /**
   * Resize the render layer.
   */
  resize(terminal: ITerminal, dim: IRenderDimensions): void;

  /**
   * Clear the state of the render layer.
   */
  reset(terminal: ITerminal): void;
}

export interface ICharacterJoiner {
  id: number;
  handler: CharacterJoinerHandler;
}

export interface ICharacterJoinerRegistry {
  registerCharacterJoiner(handler: (text: string) => [number, number][]): number;
  deregisterCharacterJoiner(joinerId: number): boolean;
  getJoinedCharacters(row: number): [number, number][];
}
