import { Terminal } from 'xterm';
import { ISearchOptions } from './Interfaces';
export declare function findNext(terminal: Terminal, term: string, searchOptions?: ISearchOptions): boolean;
export declare function findPrevious(terminal: Terminal, term: string, searchOptions: ISearchOptions): boolean;
export declare function apply(terminalConstructor: typeof Terminal): void;
