/**
 * Copyright (c) 2017 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { Terminal, ILinkMatcherOptions } from 'xterm';

const protocolClause = '(https?:\\/\\/)';
const domainCharacterSet = '[\\da-z\\.-]+';
const negatedDomainCharacterSet = '[^\\da-z\\.-]+';
const domainBodyClause = '(' + domainCharacterSet + ')';
const tldClause = '([a-z\\.]{2,6})';
const ipClause = '((\\d{1,3}\\.){3}\\d{1,3})';
const localHostClause = '(localhost)';
const portClause = '(:\\d{1,5})';
const hostClause = '((' + domainBodyClause + '\\.' + tldClause + ')|' + ipClause + '|' + localHostClause + ')' + portClause + '?';
const pathCharacterSet = '(\\/[\\/\\w\\.\\-%~:+]*)*([^:"\'\\s])';
const pathClause = '(' + pathCharacterSet + ')?';
const queryStringHashFragmentCharacterSet = '[0-9\\w\\[\\]\\(\\)\\/\\?\\!#@$%&\'*+,:;~\\=\\.\\-]*';
const queryStringClause = '(\\?' + queryStringHashFragmentCharacterSet + ')?';
const hashFragmentClause = '(#' + queryStringHashFragmentCharacterSet + ')?';
const negatedPathCharacterSet = '[^\\/\\w\\.\\-%]+';
const bodyClause = hostClause + pathClause + queryStringClause + hashFragmentClause;
const start = '(?:^|' + negatedDomainCharacterSet + ')(';
const end = ')($|' + negatedPathCharacterSet + ')';
const strictUrlRegex = new RegExp(start + protocolClause + bodyClause + end);

function handleLink(event: MouseEvent, uri: string): void {
  window.open(uri, '_blank');
}

/**
 * Initialize the web links addon, registering the link matcher.
 * @param term The terminal to use web links within.
 * @param handler A custom handler to use.
 * @param options Custom options to use, matchIndex will always be ignored.
 */
export function webLinksInit(term: Terminal, handler: (event: MouseEvent, uri: string) => void = handleLink, options: ILinkMatcherOptions = {}): void {
  options.matchIndex = 1;
  term.registerLinkMatcher(strictUrlRegex, handler, options);
}

export function apply(terminalConstructor: typeof Terminal): void {
  (<any>terminalConstructor.prototype).webLinksInit = function (handler?: (event: MouseEvent, uri: string) => void, options?: ILinkMatcherOptions): void {
    webLinksInit(this, handler, options);
  };
}
