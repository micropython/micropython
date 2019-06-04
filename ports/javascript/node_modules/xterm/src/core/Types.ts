/**
 * Copyright (c) 2018 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { IDisposable } from '../common/Types';

export const enum KeyboardResultType {
  SEND_KEY,
  SELECT_ALL,
  PAGE_UP,
  PAGE_DOWN
}

export interface IKeyboardResult {
  type: KeyboardResultType;
  cancel: boolean;
  key: string | undefined;
}

export interface ICharset {
  [key: string]: string;
}

export type CharData = [number, string, number, number];
export type IColorRGB = [number, number, number];

/** Attribute data */
export interface IAttributeData {
  fg: number;
  bg: number;

  clone(): IAttributeData;

  // flags
  isInverse(): number;
  isBold(): number;
  isUnderline(): number;
  isBlink(): number;
  isInvisible(): number;
  isItalic(): number;
  isDim(): number;

  // color modes
  getFgColorMode(): number;
  getBgColorMode(): number;
  isFgRGB(): boolean;
  isBgRGB(): boolean;
  isFgPalette(): boolean;
  isBgPalette(): boolean;
  isFgDefault(): boolean;
  isBgDefault(): boolean;

  // colors
  getFgColor(): number;
  getBgColor(): number;
}

/** Cell data */
export interface ICellData extends IAttributeData {
  content: number;
  combinedData: string;
  isCombined(): number;
  getWidth(): number;
  getChars(): string;
  getCode(): number;
  setFromCharData(value: CharData): void;
  getAsCharData(): CharData;
}

/**
 * Interface for a line in the terminal buffer.
 */
export interface IBufferLine {
  length: number;
  isWrapped: boolean;
  get(index: number): CharData;
  set(index: number, value: CharData): void;
  loadCell(index: number, cell: ICellData): ICellData;
  setCell(index: number, cell: ICellData): void;
  setCellFromCodePoint(index: number, codePoint: number, width: number, fg: number, bg: number): void;
  addCodepointToCell(index: number, codePoint: number): void;
  insertCells(pos: number, n: number, ch: ICellData): void;
  deleteCells(pos: number, n: number, fill: ICellData): void;
  replaceCells(start: number, end: number, fill: ICellData): void;
  resize(cols: number, fill: ICellData): void;
  fill(fillCellData: ICellData): void;
  copyFrom(line: IBufferLine): void;
  clone(): IBufferLine;
  getTrimmedLength(): number;
  translateToString(trimRight?: boolean, startCol?: number, endCol?: number): string;

  /* direct access to cell attrs */
  getWidth(index: number): number;
  hasWidth(index: number): number;
  getFg(index: number): number;
  getBg(index: number): number;
  hasContent(index: number): number;
  getCodePoint(index: number): number;
  isCombined(index: number): number;
  getString(index: number): string;
}

export interface IMarker extends IDisposable {
  readonly id: number;
  readonly isDisposed: boolean;
  readonly line: number;
}
