import { ISearchHelper, ISearchAddonTerminal, ISearchOptions, ISearchResult } from './Interfaces';
export declare class SearchHelper implements ISearchHelper {
    private _terminal;
    private _linesCache;
    private _linesCacheTimeoutId;
    private _cursorMoveListener;
    private _resizeListener;
    constructor(_terminal: ISearchAddonTerminal);
    findNext(term: string, searchOptions?: ISearchOptions): boolean;
    findPrevious(term: string, searchOptions?: ISearchOptions): boolean;
    private _initLinesCache;
    private _destroyLinesCache;
    private _isWholeWord;
    protected _findInLine(term: string, row: number, col: number, searchOptions?: ISearchOptions, isReverseSearch?: boolean): ISearchResult;
    translateBufferLineToStringWithWrap(lineIndex: number, trimRight: boolean): string;
    private _selectResult;
}
