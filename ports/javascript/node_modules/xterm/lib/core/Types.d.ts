import { IDisposable } from '../common/Types';
export declare const enum KeyboardResultType {
    SEND_KEY = 0,
    SELECT_ALL = 1,
    PAGE_UP = 2,
    PAGE_DOWN = 3
}
export interface IKeyboardResult {
    type: KeyboardResultType;
    cancel: boolean;
    key: string | undefined;
}
export interface ICharset {
    [key: string]: string;
}
export declare type CharData = [number, string, number, number];
export declare type IColorRGB = [number, number, number];
export interface IAttributeData {
    fg: number;
    bg: number;
    clone(): IAttributeData;
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
