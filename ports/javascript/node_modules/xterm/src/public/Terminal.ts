/**
 * Copyright (c) 2018 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { Terminal as ITerminalApi, ITerminalOptions, IMarker, IDisposable, ILinkMatcherOptions, ITheme, ILocalizableStrings, ITerminalAddon, ISelectionPosition, IBuffer as IBufferApi, IBufferLine as IBufferLineApi, IBufferCell as IBufferCellApi } from 'xterm';
import { ITerminal, IBuffer } from '../Types';
import { IBufferLine } from '../core/Types';
import { Terminal as TerminalCore } from '../Terminal';
import * as Strings from '../Strings';
import { IEvent } from '../common/EventEmitter2';
import { AddonManager } from './AddonManager';

export class Terminal implements ITerminalApi {
  private _core: ITerminal;
  private _addonManager: AddonManager;

  constructor(options?: ITerminalOptions) {
    this._core = new TerminalCore(options);
    this._addonManager = new AddonManager();
  }

  public get onCursorMove(): IEvent<void> { return this._core.onCursorMove; }
  public get onLineFeed(): IEvent<void> { return this._core.onLineFeed; }
  public get onSelectionChange(): IEvent<void> { return this._core.onSelectionChange; }
  public get onData(): IEvent<string> { return this._core.onData; }
  public get onTitleChange(): IEvent<string> { return this._core.onTitleChange; }
  public get onScroll(): IEvent<number> { return this._core.onScroll; }
  public get onKey(): IEvent<{ key: string, domEvent: KeyboardEvent }> { return this._core.onKey; }
  public get onRender(): IEvent<{ start: number, end: number }> { return this._core.onRender; }
  public get onResize(): IEvent<{ cols: number, rows: number }> { return this._core.onResize; }

  public get element(): HTMLElement { return this._core.element; }
  public get textarea(): HTMLTextAreaElement { return this._core.textarea; }
  public get rows(): number { return this._core.rows; }
  public get cols(): number { return this._core.cols; }
  public get buffer(): IBufferApi { return new BufferApiView(this._core.buffer); }
  public get markers(): ReadonlyArray<IMarker> { return this._core.markers; }
  public blur(): void {
    this._core.blur();
  }
  public focus(): void {
    this._core.focus();
  }
  public on(type: 'blur' | 'focus' | 'linefeed' | 'selection', listener: () => void): void;
  public on(type: 'data', listener: (...args: any[]) => void): void;
  public on(type: 'key', listener: (key?: string, event?: KeyboardEvent) => void): void;
  public on(type: 'keypress' | 'keydown', listener: (event?: KeyboardEvent) => void): void;
  public on(type: 'refresh', listener: (data?: { start: number; end: number; }) => void): void;
  public on(type: 'resize', listener: (data?: { cols: number; rows: number; }) => void): void;
  public on(type: 'scroll', listener: (ydisp?: number) => void): void;
  public on(type: 'title', listener: (title?: string) => void): void;
  public on(type: string, listener: (...args: any[]) => void): void;
  public on(type: any, listener: any): void {
    this._core.on(type, listener);
  }
  public off(type: string, listener: (...args: any[]) => void): void {
    this._core.off(type, listener);
  }
  public emit(type: string, data?: any): void {
    this._core.emit(type, data);
  }
  public addDisposableListener(type: string, handler: (...args: any[]) => void): IDisposable {
    return this._core.addDisposableListener(type, handler);
  }
  public resize(columns: number, rows: number): void {
    this._core.resize(columns, rows);
  }
  public writeln(data: string): void {
    this._core.writeln(data);
  }
  public open(parent: HTMLElement): void {
    this._core.open(parent);
  }
  public attachCustomKeyEventHandler(customKeyEventHandler: (event: KeyboardEvent) => boolean): void {
    this._core.attachCustomKeyEventHandler(customKeyEventHandler);
  }
  public addCsiHandler(flag: string, callback: (params: number[], collect: string) => boolean): IDisposable {
    return this._core.addCsiHandler(flag, callback);
  }
  public addOscHandler(ident: number, callback: (data: string) => boolean): IDisposable {
    return this._core.addOscHandler(ident, callback);
  }
  public registerLinkMatcher(regex: RegExp, handler: (event: MouseEvent, uri: string) => void, options?: ILinkMatcherOptions): number {
    return this._core.registerLinkMatcher(regex, handler, options);
  }
  public deregisterLinkMatcher(matcherId: number): void {
    this._core.deregisterLinkMatcher(matcherId);
  }
  public registerCharacterJoiner(handler: (text: string) => [number, number][]): number {
    return this._core.registerCharacterJoiner(handler);
  }
  public deregisterCharacterJoiner(joinerId: number): void {
    this._core.deregisterCharacterJoiner(joinerId);
  }
  public addMarker(cursorYOffset: number): IMarker {
    return this._core.addMarker(cursorYOffset);
  }
  public hasSelection(): boolean {
    return this._core.hasSelection();
  }
  public select(column: number, row: number, length: number): void {
    this._core.select(column, row, length);
  }
  public getSelection(): string {
    return this._core.getSelection();
  }
  public getSelectionPosition(): ISelectionPosition | undefined {
    return this._core.getSelectionPosition();
  }
  public clearSelection(): void {
    this._core.clearSelection();
  }
  public selectAll(): void {
    this._core.selectAll();
  }
  public selectLines(start: number, end: number): void {
    this._core.selectLines(start, end);
  }
  public dispose(): void {
    this._addonManager.dispose();
    this._core.dispose();
  }
  public destroy(): void {
    this._core.destroy();
  }
  public scrollLines(amount: number): void {
    this._core.scrollLines(amount);
  }
  public scrollPages(pageCount: number): void {
    this._core.scrollPages(pageCount);
  }
  public scrollToTop(): void {
    this._core.scrollToTop();
  }
  public scrollToBottom(): void {
    this._core.scrollToBottom();
  }
  public scrollToLine(line: number): void {
    this._core.scrollToLine(line);
  }
  public clear(): void {
    this._core.clear();
  }
  public write(data: string): void {
    this._core.write(data);
  }
  public writeUtf8(data: Uint8Array): void {
    this._core.writeUtf8(data);
  }
  public getOption(key: 'bellSound' | 'bellStyle' | 'cursorStyle' | 'fontFamily' | 'fontWeight' | 'fontWeightBold' | 'rendererType' | 'termName'): string;
  public getOption(key: 'allowTransparency' | 'cancelEvents' | 'convertEol' | 'cursorBlink' | 'debug' | 'disableStdin' | 'enableBold' | 'macOptionIsMeta' | 'rightClickSelectsWord' | 'popOnBell' | 'screenKeys' | 'useFlowControl' | 'visualBell'): boolean;
  public getOption(key: 'colors'): string[];
  public getOption(key: 'cols' | 'fontSize' | 'letterSpacing' | 'lineHeight' | 'rows' | 'tabStopWidth' | 'scrollback'): number;
  public getOption(key: 'handler'): (data: string) => void;
  public getOption(key: string): any;
  public getOption(key: any): any {
    return this._core.getOption(key);
  }
  public setOption(key: 'bellSound' | 'fontFamily' | 'termName', value: string): void;
  public setOption(key: 'fontWeight' | 'fontWeightBold', value: 'normal' | 'bold' | '100' | '200' | '300' | '400' | '500' | '600' | '700' | '800' | '900'): void;
  public setOption(key: 'bellStyle', value: 'none' | 'visual' | 'sound' | 'both'): void;
  public setOption(key: 'cursorStyle', value: 'block' | 'underline' | 'bar'): void;
  public setOption(key: 'allowTransparency' | 'cancelEvents' | 'convertEol' | 'cursorBlink' | 'debug' | 'disableStdin' | 'enableBold' | 'macOptionIsMeta' | 'rightClickSelectsWord' | 'popOnBell' | 'screenKeys' | 'useFlowControl' | 'visualBell', value: boolean): void;
  public setOption(key: 'colors', value: string[]): void;
  public setOption(key: 'fontSize' | 'letterSpacing' | 'lineHeight' | 'tabStopWidth' | 'scrollback', value: number): void;
  public setOption(key: 'handler', value: (data: string) => void): void;
  public setOption(key: 'theme', value: ITheme): void;
  public setOption(key: 'cols' | 'rows', value: number): void;
  public setOption(key: string, value: any): void;
  public setOption(key: any, value: any): void {
    this._core.setOption(key, value);
  }
  public refresh(start: number, end: number): void {
    this._core.refresh(start, end);
  }
  public reset(): void {
    this._core.reset();
  }
  public static applyAddon(addon: any): void {
    addon.apply(Terminal);
  }
  public loadAddon(addon: ITerminalAddon): void {
    return this._addonManager.loadAddon(this, addon);
  }
  public static get strings(): ILocalizableStrings {
    return Strings;
  }
}

class BufferApiView implements IBufferApi {
  constructor(private _buffer: IBuffer) {}

  public get cursorY(): number { return this._buffer.y; }
  public get cursorX(): number { return this._buffer.x; }
  public get viewportY(): number { return this._buffer.ydisp; }
  public get baseY(): number { return this._buffer.ybase; }
  public get length(): number { return this._buffer.lines.length; }
  public getLine(y: number): IBufferLineApi | undefined {
    const line = this._buffer.lines.get(y);
    if (!line) {
      return undefined;
    }
    return new BufferLineApiView(line);
  }
}

class BufferLineApiView implements IBufferLineApi {
  constructor(private _line: IBufferLine) {}

  public get isWrapped(): boolean { return this._line.isWrapped; }
  public getCell(x: number): IBufferCellApi | undefined {
    if (x < 0 || x >= this._line.length) {
      return undefined;
    }
    return new BufferCellApiView(this._line, x);
  }
  public translateToString(trimRight?: boolean, startColumn?: number, endColumn?: number): string {
    return this._line.translateToString(trimRight, startColumn, endColumn);
  }
}

class BufferCellApiView implements IBufferCellApi {
  constructor(private _line: IBufferLine, private _x: number) {}
  public get char(): string { return this._line.getString(this._x); }
  public get width(): number { return this._line.getWidth(this._x); }
}
