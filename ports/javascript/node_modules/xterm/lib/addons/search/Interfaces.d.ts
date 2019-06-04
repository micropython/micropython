import { Terminal } from 'xterm';
export interface ISearchAddonTerminal extends Terminal {
    __searchHelper?: ISearchHelper;
}
export interface ISearchHelper {
    findNext(term: string, searchOptions: ISearchOptions): boolean;
    findPrevious(term: string, searchOptions: ISearchOptions): boolean;
}
export interface ISearchOptions {
    regex?: boolean;
    wholeWord?: boolean;
    caseSensitive?: boolean;
    incremental?: boolean;
}
export interface ISearchResult {
    term: string;
    col: number;
    row: number;
}
