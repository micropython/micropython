/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { SearchHelper } from './SearchHelper';
import { Terminal } from 'xterm';
import { ISearchAddonTerminal, ISearchOptions } from './Interfaces';

/**
 * Find the next instance of the term, then scroll to and select it. If it
 * doesn't exist, do nothing.
 * @param term The search term.
 * @param searchOptions Search options
 * @return Whether a result was found.
 */
export function findNext(terminal: Terminal, term: string, searchOptions: ISearchOptions = {}): boolean {
  const addonTerminal = <ISearchAddonTerminal>terminal;
  if (!addonTerminal.__searchHelper) {
    addonTerminal.__searchHelper = new SearchHelper(addonTerminal);
  }
  return addonTerminal.__searchHelper.findNext(term, searchOptions);
}

/**
 * Find the previous instance of the term, then scroll to and select it. If it
 * doesn't exist, do nothing.
 * @param term The search term.
 * @param searchOptions Search options
 * @return Whether a result was found.
 */
export function findPrevious(terminal: Terminal, term: string, searchOptions: ISearchOptions): boolean {
  const addonTerminal = <ISearchAddonTerminal>terminal;
  if (!addonTerminal.__searchHelper) {
    addonTerminal.__searchHelper = new SearchHelper(addonTerminal);
  }
  return addonTerminal.__searchHelper.findPrevious(term, searchOptions);
}

export function apply(terminalConstructor: typeof Terminal): void {
  (<any>terminalConstructor.prototype).findNext = function(term: string, searchOptions: ISearchOptions): boolean {
    return findNext(this, term, searchOptions);
  };

  (<any>terminalConstructor.prototype).findPrevious = function(term: string, searchOptions: ISearchOptions): boolean {
    return findPrevious(this, term, searchOptions);
  };
}
