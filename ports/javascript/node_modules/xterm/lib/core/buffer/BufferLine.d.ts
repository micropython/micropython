import { CharData, IBufferLine, ICellData, IColorRGB, IAttributeData } from '../Types';
export declare const DEFAULT_ATTR: number;
export declare const CHAR_DATA_ATTR_INDEX = 0;
export declare const CHAR_DATA_CHAR_INDEX = 1;
export declare const CHAR_DATA_WIDTH_INDEX = 2;
export declare const CHAR_DATA_CODE_INDEX = 3;
export declare const NULL_CELL_CHAR = "";
export declare const NULL_CELL_WIDTH = 1;
export declare const NULL_CELL_CODE = 0;
export declare const WHITESPACE_CELL_CHAR = " ";
export declare const WHITESPACE_CELL_WIDTH = 1;
export declare const WHITESPACE_CELL_CODE = 32;
export declare const enum Content {
    CODEPOINT_MASK = 2097151,
    IS_COMBINED_MASK = 2097152,
    HAS_CONTENT_MASK = 4194303,
    WIDTH_MASK = 12582912,
    WIDTH_SHIFT = 22
}
export declare const enum Attributes {
    BLUE_MASK = 255,
    BLUE_SHIFT = 0,
    PCOLOR_MASK = 255,
    PCOLOR_SHIFT = 0,
    GREEN_MASK = 65280,
    GREEN_SHIFT = 8,
    RED_MASK = 16711680,
    RED_SHIFT = 16,
    CM_MASK = 50331648,
    CM_DEFAULT = 0,
    CM_P16 = 16777216,
    CM_P256 = 33554432,
    CM_RGB = 50331648,
    RGB_MASK = 16777215
}
export declare const enum FgFlags {
    INVERSE = 67108864,
    BOLD = 134217728,
    UNDERLINE = 268435456,
    BLINK = 536870912,
    INVISIBLE = 1073741824
}
export declare const enum BgFlags {
    ITALIC = 67108864,
    DIM = 134217728
}
export declare class AttributeData implements IAttributeData {
    static toColorRGB(value: number): IColorRGB;
    static fromColorRGB(value: IColorRGB): number;
    clone(): IAttributeData;
    fg: number;
    bg: number;
    isInverse(): number;
    isBold(): number;
    isUnderline(): number;
    isBlink(): number;
    isInvisible(): number;
    isItalic(): number;
    isDim(): number;
    getFgColorMode(): number;
    getBgColorMode(): number;
    isFgRGB(): boolean;
    isBgRGB(): boolean;
    isFgPalette(): boolean;
    isBgPalette(): boolean;
    isFgDefault(): boolean;
    isBgDefault(): boolean;
    getFgColor(): number;
    getBgColor(): number;
}
export declare const DEFAULT_ATTR_DATA: AttributeData;
export declare class CellData extends AttributeData implements ICellData {
    static fromCharData(value: CharData): CellData;
    content: number;
    fg: number;
    bg: number;
    combinedData: string;
    isCombined(): number;
    getWidth(): number;
    getChars(): string;
    getCode(): number;
    setFromCharData(value: CharData): void;
    getAsCharData(): CharData;
}
export declare class BufferLine implements IBufferLine {
    isWrapped: boolean;
    protected _data: Uint32Array;
    protected _combined: {
        [index: number]: string;
    };
    length: number;
    constructor(cols: number, fillCellData?: ICellData, isWrapped?: boolean);
    get(index: number): CharData;
    set(index: number, value: CharData): void;
    getWidth(index: number): number;
    hasWidth(index: number): number;
    getFg(index: number): number;
    getBg(index: number): number;
    hasContent(index: number): number;
    getCodePoint(index: number): number;
    isCombined(index: number): number;
    getString(index: number): string;
    loadCell(index: number, cell: ICellData): ICellData;
    setCell(index: number, cell: ICellData): void;
    setCellFromCodePoint(index: number, codePoint: number, width: number, fg: number, bg: number): void;
    addCodepointToCell(index: number, codePoint: number): void;
    insertCells(pos: number, n: number, fillCellData: ICellData): void;
    deleteCells(pos: number, n: number, fillCellData: ICellData): void;
    replaceCells(start: number, end: number, fillCellData: ICellData): void;
    resize(cols: number, fillCellData: ICellData): void;
    fill(fillCellData: ICellData): void;
    copyFrom(line: BufferLine): void;
    clone(): IBufferLine;
    getTrimmedLength(): number;
    copyCellsFrom(src: BufferLine, srcCol: number, destCol: number, length: number, applyInReverse: boolean): void;
    translateToString(trimRight?: boolean, startCol?: number, endCol?: number): string;
}
