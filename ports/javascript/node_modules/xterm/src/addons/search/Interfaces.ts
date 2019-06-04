/**
 * Copyright (c) 2018 The xterm.js authors. All rights reserved.
 * @license MIT
 */

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
  /**
   * Use this when you want the selection to expand if it still matches as the
   * user types. Note that this only affects findNext.
   */
  incremental?: boolean;
}

export interface ISearchResult {
  term: string;
  col: number;
  row: number;
}
