/**
 * Copyright (c) 2018 The xterm.js authors. All rights reserved.
 * @license MIT
 */
import { CharData, IBufferLine, ICellData, IColorRGB, IAttributeData } from '../Types';
import { stringFromCodePoint } from '../input/TextDecoder';
import { DEFAULT_COLOR } from '../../common/Types';

export const DEFAULT_ATTR = (0 << 18) | (DEFAULT_COLOR << 9) | (256 << 0);

export const CHAR_DATA_ATTR_INDEX = 0;
export const CHAR_DATA_CHAR_INDEX = 1;
export const CHAR_DATA_WIDTH_INDEX = 2;
export const CHAR_DATA_CODE_INDEX = 3;

/**
 * Null cell - a real empty cell (containing nothing).
 * Note that code should always be 0 for a null cell as
 * several test condition of the buffer line rely on this.
 */
export const NULL_CELL_CHAR = '';
export const NULL_CELL_WIDTH = 1;
export const NULL_CELL_CODE = 0;

/**
 * Whitespace cell.
 * This is meant as a replacement for empty cells when needed
 * during rendering lines to preserve correct aligment.
 */
export const WHITESPACE_CELL_CHAR = ' ';
export const WHITESPACE_CELL_WIDTH = 1;
export const WHITESPACE_CELL_CODE = 32;

/**
 * buffer memory layout:
 *
 *   |             uint32_t             |        uint32_t         |        uint32_t         |
 *   |             `content`            |          `FG`           |          `BG`           |
 *   | wcwidth(2) comb(1) codepoint(21) | flags(8) R(8) G(8) B(8) | flags(8) R(8) G(8) B(8) |
 */


/** typed array slots taken by one cell */
const CELL_SIZE = 3;

/**
 * Cell member indices.
 *
 * Direct access:
 *    `content = data[column * CELL_SIZE + Cell.CONTENT];`
 *    `fg = data[column * CELL_SIZE + Cell.FG];`
 *    `bg = data[column * CELL_SIZE + Cell.BG];`
 */
const enum Cell {
  CONTENT = 0,
  FG = 1, // currently simply holds all known attrs
  BG = 2  // currently unused
}

/**
 * Bitmasks for accessing data in `content`.
 */
export const enum Content {
  /**
   * bit 1..21    codepoint, max allowed in UTF32 is 0x10FFFF (21 bits taken)
   *              read:   `codepoint = content & Content.codepointMask;`
   *              write:  `content |= codepoint & Content.codepointMask;`
   *                      shortcut if precondition `codepoint <= 0x10FFFF` is met:
   *                      `content |= codepoint;`
   */
  CODEPOINT_MASK = 0x1FFFFF,

  /**
   * bit 22       flag indication whether a cell contains combined content
   *              read:   `isCombined = content & Content.isCombined;`
   *              set:    `content |= Content.isCombined;`
   *              clear:  `content &= ~Content.isCombined;`
   */
  IS_COMBINED_MASK = 0x200000,  // 1 << 21

  /**
   * bit 1..22    mask to check whether a cell contains any string data
   *              we need to check for codepoint and isCombined bits to see
   *              whether a cell contains anything
   *              read:   `isEmpty = !(content & Content.hasContent)`
   */
  HAS_CONTENT_MASK = 0x3FFFFF,

  /**
   * bit 23..24   wcwidth value of cell, takes 2 bits (ranges from 0..2)
   *              read:   `width = (content & Content.widthMask) >> Content.widthShift;`
   *                      `hasWidth = content & Content.widthMask;`
   *                      as long as wcwidth is highest value in `content`:
   *                      `width = content >> Content.widthShift;`
   *              write:  `content |= (width << Content.widthShift) & Content.widthMask;`
   *                      shortcut if precondition `0 <= width <= 3` is met:
   *                      `content |= width << Content.widthShift;`
   */
  WIDTH_MASK = 0xC00000,   // 3 << 22
  WIDTH_SHIFT = 22
}


export const enum Attributes {
  /**
   * bit 1..8     blue in RGB, color in P256 and P16
   */
  BLUE_MASK = 0xFF,
  BLUE_SHIFT = 0,
  PCOLOR_MASK = 0xFF,
  PCOLOR_SHIFT = 0,

  /**
   * bit 9..16    green in RGB
   */
  GREEN_MASK = 0xFF00,
  GREEN_SHIFT = 8,

  /**
   * bit 17..24   red in RGB
   */
  RED_MASK = 0xFF0000,
  RED_SHIFT = 16,

  /**
   * bit 25..26   color mode: DEFAULT (0) | P16 (1) | P256 (2) | RGB (3)
   */
  CM_MASK = 0x3000000,
  CM_DEFAULT = 0,
  CM_P16 = 0x1000000,
  CM_P256 = 0x2000000,
  CM_RGB = 0x3000000,

  /**
   * bit 1..24  RGB room
   */
  RGB_MASK = 0xFFFFFF
}

export const enum FgFlags {
  /**
   * bit 27..31 (32th bit unused)
   */
  INVERSE = 0x4000000,
  BOLD = 0x8000000,
  UNDERLINE = 0x10000000,
  BLINK = 0x20000000,
  INVISIBLE = 0x40000000
}

export const enum BgFlags {
  /**
   * bit 27..32 (upper 4 unused)
   */
  ITALIC = 0x4000000,
  DIM = 0x8000000
}

export class AttributeData implements IAttributeData {
  static toColorRGB(value: number): IColorRGB {
    return [
      value >>> Attributes.RED_SHIFT & 255,
      value >>> Attributes.GREEN_SHIFT & 255,
      value & 255
    ];
  }
  static fromColorRGB(value: IColorRGB): number {
    return (value[0] & 255) << Attributes.RED_SHIFT | (value[1] & 255) << Attributes.GREEN_SHIFT | value[2] & 255;
  }

  public clone(): IAttributeData {
    const newObj = new AttributeData();
    newObj.fg = this.fg;
    newObj.bg = this.bg;
    return newObj;
  }

  // data
  public fg: number = 0;
  public bg: number = 0;

  // flags
  public isInverse(): number   { return this.fg & FgFlags.INVERSE; }
  public isBold(): number      { return this.fg & FgFlags.BOLD; }
  public isUnderline(): number { return this.fg & FgFlags.UNDERLINE; }
  public isBlink(): number     { return this.fg & FgFlags.BLINK; }
  public isInvisible(): number { return this.fg & FgFlags.INVISIBLE; }
  public isItalic(): number    { return this.bg & BgFlags.ITALIC; }
  public isDim(): number       { return this.bg & BgFlags.DIM; }

  // color modes
  public getFgColorMode(): number { return this.fg & Attributes.CM_MASK; }
  public getBgColorMode(): number { return this.bg & Attributes.CM_MASK; }
  public isFgRGB(): boolean       { return (this.fg & Attributes.CM_MASK) === Attributes.CM_RGB; }
  public isBgRGB(): boolean       { return (this.bg & Attributes.CM_MASK) === Attributes.CM_RGB; }
  public isFgPalette(): boolean   { return (this.fg & Attributes.CM_MASK) === Attributes.CM_P16 || (this.fg & Attributes.CM_MASK) === Attributes.CM_P256; }
  public isBgPalette(): boolean   { return (this.bg & Attributes.CM_MASK) === Attributes.CM_P16 || (this.bg & Attributes.CM_MASK) === Attributes.CM_P256; }
  public isFgDefault(): boolean   { return (this.fg & Attributes.CM_MASK) === 0; }
  public isBgDefault(): boolean   { return (this.bg & Attributes.CM_MASK) === 0; }

  // colors
  public getFgColor(): number {
    switch (this.fg & Attributes.CM_MASK) {
      case Attributes.CM_P16:
      case Attributes.CM_P256:  return this.fg & Attributes.PCOLOR_MASK;
      case Attributes.CM_RGB:   return this.fg & Attributes.RGB_MASK;
      default:                  return -1;  // CM_DEFAULT defaults to -1
    }
  }
  public getBgColor(): number {
    switch (this.bg & Attributes.CM_MASK) {
      case Attributes.CM_P16:
      case Attributes.CM_P256:  return this.bg & Attributes.PCOLOR_MASK;
      case Attributes.CM_RGB:   return this.bg & Attributes.RGB_MASK;
      default:                  return -1;  // CM_DEFAULT defaults to -1
    }
  }
}

export const DEFAULT_ATTR_DATA = new AttributeData();

/**
 * CellData - represents a single Cell in the terminal buffer.
 */
export class CellData extends AttributeData implements ICellData {

  /** Helper to create CellData from CharData. */
  public static fromCharData(value: CharData): CellData {
    const obj = new CellData();
    obj.setFromCharData(value);
    return obj;
  }

  /** Primitives from terminal buffer. */
  public content: number = 0;
  public fg: number = 0;
  public bg: number = 0;
  public combinedData: string = '';

  /** Whether cell contains a combined string. */
  public isCombined(): number {
    return this.content & Content.IS_COMBINED_MASK;
  }

  /** Width of the cell. */
  public getWidth(): number {
    return this.content >> Content.WIDTH_SHIFT;
  }

  /** JS string of the content. */
  public getChars(): string {
    if (this.content & Content.IS_COMBINED_MASK) {
      return this.combinedData;
    }
    if (this.content & Content.CODEPOINT_MASK) {
      return stringFromCodePoint(this.content & Content.CODEPOINT_MASK);
    }
    return '';
  }

  /**
   * Codepoint of cell
   * Note this returns the UTF32 codepoint of single chars,
   * if content is a combined string it returns the codepoint
   * of the last char in string to be in line with code in CharData.
   * */
  public getCode(): number {
    return (this.isCombined())
      ? this.combinedData.charCodeAt(this.combinedData.length - 1)
      : this.content & Content.CODEPOINT_MASK;
  }

  /** Set data from CharData */
  public setFromCharData(value: CharData): void {
    this.fg = value[CHAR_DATA_ATTR_INDEX];
    this.bg = 0;
    let combined = false;

    // surrogates and combined strings need special treatment
    if (value[CHAR_DATA_CHAR_INDEX].length > 2) {
      combined = true;
    } else if (value[CHAR_DATA_CHAR_INDEX].length === 2) {
      const code = value[CHAR_DATA_CHAR_INDEX].charCodeAt(0);
      // if the 2-char string is a surrogate create single codepoint
      // everything else is combined
      if (0xD800 <= code && code <= 0xDBFF) {
        const second = value[CHAR_DATA_CHAR_INDEX].charCodeAt(1);
        if (0xDC00 <= second && second <= 0xDFFF) {
          this.content = ((code - 0xD800) * 0x400 + second - 0xDC00 + 0x10000) | (value[CHAR_DATA_WIDTH_INDEX] << Content.WIDTH_SHIFT);
        } else {
          combined = true;
        }
      } else {
        combined = true;
      }
    } else {
      this.content = value[CHAR_DATA_CHAR_INDEX].charCodeAt(0) | (value[CHAR_DATA_WIDTH_INDEX] << Content.WIDTH_SHIFT);
    }
    if (combined) {
      this.combinedData = value[CHAR_DATA_CHAR_INDEX];
      this.content = Content.IS_COMBINED_MASK | (value[CHAR_DATA_WIDTH_INDEX] << Content.WIDTH_SHIFT);
    }
  }

  /** Get data as CharData. */
  public getAsCharData(): CharData {
    return [this.fg, this.getChars(), this.getWidth(), this.getCode()];
  }
}


/**
 * Typed array based bufferline implementation.
 *
 * There are 2 ways to insert data into the cell buffer:
 * - `setCellFromCodepoint` + `addCodepointToCell`
 *   Use these for data that is already UTF32.
 *   Used during normal input in `InputHandler` for faster buffer access.
 * - `setCell`
 *   This method takes a CellData object and stores the data in the buffer.
 *   Use `CellData.fromCharData` to create the CellData object (e.g. from JS string).
 *
 * To retrieve data from the buffer use either one of the primitive methods
 * (if only one particular value is needed) or `loadCell`. For `loadCell` in a loop
 * memory allocs / GC pressure can be greatly reduced by reusing the CellData object.
 */
export class BufferLine implements IBufferLine {
  protected _data: Uint32Array;
  protected _combined: {[index: number]: string} = {};
  public length: number;

  constructor(cols: number, fillCellData?: ICellData, public isWrapped: boolean = false) {
    this._data = new Uint32Array(cols * CELL_SIZE);
    const cell = fillCellData || CellData.fromCharData([0, NULL_CELL_CHAR, NULL_CELL_WIDTH, NULL_CELL_CODE]);
    for (let i = 0; i < cols; ++i) {
      this.setCell(i, cell);
    }
    this.length = cols;
  }

  /**
   * Get cell data CharData.
   * @deprecated
   */
  public get(index: number): CharData {
    const content = this._data[index * CELL_SIZE + Cell.CONTENT];
    const cp = content & Content.CODEPOINT_MASK;
    return [
      this._data[index * CELL_SIZE + Cell.FG],
      (content & Content.IS_COMBINED_MASK)
        ? this._combined[index]
        : (cp) ? stringFromCodePoint(cp) : '',
      content >> Content.WIDTH_SHIFT,
      (content & Content.IS_COMBINED_MASK)
        ? this._combined[index].charCodeAt(this._combined[index].length - 1)
        : cp
    ];
  }

  /**
   * Set cell data from CharData.
   * @deprecated
   */
  public set(index: number, value: CharData): void {
    this._data[index * CELL_SIZE + Cell.FG] = value[CHAR_DATA_ATTR_INDEX];
    if (value[CHAR_DATA_CHAR_INDEX].length > 1) {
      this._combined[index] = value[1];
      this._data[index * CELL_SIZE + Cell.CONTENT] = index | Content.IS_COMBINED_MASK | (value[CHAR_DATA_WIDTH_INDEX] << Content.WIDTH_SHIFT);
    } else {
      this._data[index * CELL_SIZE + Cell.CONTENT] = value[CHAR_DATA_CHAR_INDEX].charCodeAt(0) | (value[CHAR_DATA_WIDTH_INDEX] << Content.WIDTH_SHIFT);
    }
  }

  /**
   * primitive getters
   * use these when only one value is needed, otherwise use `loadCell`
   */
  public getWidth(index: number): number {
    return this._data[index * CELL_SIZE + Cell.CONTENT] >> Content.WIDTH_SHIFT;
  }

  /** Test whether content has width. */
  public hasWidth(index: number): number {
    return this._data[index * CELL_SIZE + Cell.CONTENT] & Content.WIDTH_MASK;
  }

  /** Get FG cell component. */
  public getFg(index: number): number {
    return this._data[index * CELL_SIZE + Cell.FG];
  }

  /** Get BG cell component. */
  public getBg(index: number): number {
    return this._data[index * CELL_SIZE + Cell.BG];
  }

  /**
   * Test whether contains any chars.
   * Basically an empty has no content, but other cells might differ in FG/BG
   * from real empty cells.
   * */
  public hasContent(index: number): number {
    return this._data[index * CELL_SIZE + Cell.CONTENT] & Content.HAS_CONTENT_MASK;
  }

  /**
   * Get codepoint of the cell.
   * To be in line with `code` in CharData this either returns
   * a single UTF32 codepoint or the last codepoint of a combined string.
   */
  public getCodePoint(index: number): number {
    const content = this._data[index * CELL_SIZE + Cell.CONTENT];
    if (content & Content.IS_COMBINED_MASK) {
      return this._combined[index].charCodeAt(this._combined[index].length - 1);
    }
    return content & Content.CODEPOINT_MASK;
  }

  /** Test whether the cell contains a combined string. */
  public isCombined(index: number): number {
    return this._data[index * CELL_SIZE + Cell.CONTENT] & Content.IS_COMBINED_MASK;
  }

  /** Returns the string content of the cell. */
  public getString(index: number): string {
    const content = this._data[index * CELL_SIZE + Cell.CONTENT];
    if (content & Content.IS_COMBINED_MASK) {
      return this._combined[index];
    }
    if (content & Content.CODEPOINT_MASK) {
      return stringFromCodePoint(content & Content.CODEPOINT_MASK);
    }
    // return empty string for empty cells
    return '';
  }

  /**
   * Load data at `index` into `cell`. This is used to access cells in a way that's more friendly
   * to GC as it significantly reduced the amount of new objects/references needed.
   */
  public loadCell(index: number, cell: ICellData): ICellData {
    const startIndex = index * CELL_SIZE;
    cell.content = this._data[startIndex + Cell.CONTENT];
    cell.fg = this._data[startIndex + Cell.FG];
    cell.bg = this._data[startIndex + Cell.BG];
    if (cell.content & Content.IS_COMBINED_MASK) {
      cell.combinedData = this._combined[index];
    }
    return cell;
  }

  /**
   * Set data at `index` to `cell`.
   */
  public setCell(index: number, cell: ICellData): void {
    if (cell.content & Content.IS_COMBINED_MASK) {
      this._combined[index] = cell.combinedData;
    }
    this._data[index * CELL_SIZE + Cell.CONTENT] = cell.content;
    this._data[index * CELL_SIZE + Cell.FG] = cell.fg;
    this._data[index * CELL_SIZE + Cell.BG] = cell.bg;
  }

  /**
   * Set cell data from input handler.
   * Since the input handler see the incoming chars as UTF32 codepoints,
   * it gets an optimized access method.
   */
  public setCellFromCodePoint(index: number, codePoint: number, width: number, fg: number, bg: number): void {
    this._data[index * CELL_SIZE + Cell.CONTENT] = codePoint | (width << Content.WIDTH_SHIFT);
    this._data[index * CELL_SIZE + Cell.FG] = fg;
    this._data[index * CELL_SIZE + Cell.BG] = bg;
  }

  /**
   * Add a codepoint to a cell from input handler.
   * During input stage combining chars with a width of 0 follow and stack
   * onto a leading char. Since we already set the attrs
   * by the previous `setDataFromCodePoint` call, we can omit it here.
   */
  public addCodepointToCell(index: number, codePoint: number): void {
    let content = this._data[index * CELL_SIZE + Cell.CONTENT];
    if (content & Content.IS_COMBINED_MASK) {
      // we already have a combined string, simply add
      this._combined[index] += stringFromCodePoint(codePoint);
    } else {
      if (content & Content.CODEPOINT_MASK) {
        // normal case for combining chars:
        //  - move current leading char + new one into combined string
        //  - set combined flag
        this._combined[index] = stringFromCodePoint(content & Content.CODEPOINT_MASK) + stringFromCodePoint(codePoint);
        content &= ~Content.CODEPOINT_MASK; // set codepoint in buffer to 0
        content |= Content.IS_COMBINED_MASK;
      } else {
        // should not happen - we actually have no data in the cell yet
        // simply set the data in the cell buffer with a width of 1
        content = codePoint | (1 << Content.WIDTH_SHIFT);
      }
      this._data[index * CELL_SIZE + Cell.CONTENT] = content;
    }
  }

  public insertCells(pos: number, n: number, fillCellData: ICellData): void {
    pos %= this.length;
    if (n < this.length - pos) {
      const cell = new CellData();
      for (let i = this.length - pos - n - 1; i >= 0; --i) {
        this.setCell(pos + n + i, this.loadCell(pos + i, cell));
      }
      for (let i = 0; i < n; ++i) {
        this.setCell(pos + i, fillCellData);
      }
    } else {
      for (let i = pos; i < this.length; ++i) {
        this.setCell(i, fillCellData);
      }
    }
  }

  public deleteCells(pos: number, n: number, fillCellData: ICellData): void {
    pos %= this.length;
    if (n < this.length - pos) {
      const cell = new CellData();
      for (let i = 0; i < this.length - pos - n; ++i) {
        this.setCell(pos + i, this.loadCell(pos + n + i, cell));
      }
      for (let i = this.length - n; i < this.length; ++i) {
        this.setCell(i, fillCellData);
      }
    } else {
      for (let i = pos; i < this.length; ++i) {
        this.setCell(i, fillCellData);
      }
    }
  }

  public replaceCells(start: number, end: number, fillCellData: ICellData): void {
    while (start < end  && start < this.length) {
      this.setCell(start++, fillCellData);
    }
  }

  public resize(cols: number, fillCellData: ICellData): void {
    if (cols === this.length) {
      return;
    }
    if (cols > this.length) {
      const data = new Uint32Array(cols * CELL_SIZE);
      if (this.length) {
        if (cols * CELL_SIZE < this._data.length) {
          data.set(this._data.subarray(0, cols * CELL_SIZE));
        } else {
          data.set(this._data);
        }
      }
      this._data = data;
      for (let i = this.length; i < cols; ++i) {
        this.setCell(i, fillCellData);
      }
    } else {
      if (cols) {
        const data = new Uint32Array(cols * CELL_SIZE);
        data.set(this._data.subarray(0, cols * CELL_SIZE));
        this._data = data;
        // Remove any cut off combined data
        const keys = Object.keys(this._combined);
        for (let i = 0; i < keys.length; i++) {
          const key = parseInt(keys[i], 10);
          if (key >= cols) {
            delete this._combined[key];
          }
        }
      } else {
        this._data = new Uint32Array(0);
        this._combined = {};
      }
    }
    this.length = cols;
  }

  /** fill a line with fillCharData */
  public fill(fillCellData: ICellData): void {
    this._combined = {};
    for (let i = 0; i < this.length; ++i) {
      this.setCell(i, fillCellData);
    }
  }

  /** alter to a full copy of line  */
  public copyFrom(line: BufferLine): void {
    if (this.length !== line.length) {
      this._data = new Uint32Array(line._data);
    } else {
      // use high speed copy if lengths are equal
      this._data.set(line._data);
    }
    this.length = line.length;
    this._combined = {};
    for (const el in line._combined) {
      this._combined[el] = line._combined[el];
    }
    this.isWrapped = line.isWrapped;
  }

  /** create a new clone */
  public clone(): IBufferLine {
    const newLine = new BufferLine(0);
    newLine._data = new Uint32Array(this._data);
    newLine.length = this.length;
    for (const el in this._combined) {
      newLine._combined[el] = this._combined[el];
    }
    newLine.isWrapped = this.isWrapped;
    return newLine;
  }

  public getTrimmedLength(): number {
    for (let i = this.length - 1; i >= 0; --i) {
      if ((this._data[i * CELL_SIZE + Cell.CONTENT] & Content.HAS_CONTENT_MASK)) {
        return i + (this._data[i * CELL_SIZE + Cell.CONTENT] >> Content.WIDTH_SHIFT);
      }
    }
    return 0;
  }

  public copyCellsFrom(src: BufferLine, srcCol: number, destCol: number, length: number, applyInReverse: boolean): void {
    const srcData = src._data;
    if (applyInReverse) {
      for (let cell = length - 1; cell >= 0; cell--) {
        for (let i = 0; i < CELL_SIZE; i++) {
          this._data[(destCol + cell) * CELL_SIZE + i] = srcData[(srcCol + cell) * CELL_SIZE + i];
        }
      }
    } else {
      for (let cell = 0; cell < length; cell++) {
        for (let i = 0; i < CELL_SIZE; i++) {
          this._data[(destCol + cell) * CELL_SIZE + i] = srcData[(srcCol + cell) * CELL_SIZE + i];
        }
      }
    }

    // Move any combined data over as needed
    const srcCombinedKeys = Object.keys(src._combined);
    for (let i = 0; i < srcCombinedKeys.length; i++) {
      const key = parseInt(srcCombinedKeys[i], 10);
      if (key >= srcCol) {
        this._combined[key - srcCol + destCol] = src._combined[key];
      }
    }
  }

  public translateToString(trimRight: boolean = false, startCol: number = 0, endCol: number = this.length): string {
    if (trimRight) {
      endCol = Math.min(endCol, this.getTrimmedLength());
    }
    let result = '';
    while (startCol < endCol) {
      const content = this._data[startCol * CELL_SIZE + Cell.CONTENT];
      const cp = content & Content.CODEPOINT_MASK;
      result += (content & Content.IS_COMBINED_MASK) ? this._combined[startCol] : (cp) ? stringFromCodePoint(cp) : WHITESPACE_CELL_CHAR;
      startCol += (content >> Content.WIDTH_SHIFT) || 1; // always advance by 1
    }
    return result;
  }
}
