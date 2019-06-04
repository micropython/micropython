import { Terminal, ILinkMatcherOptions } from 'xterm';
export declare function webLinksInit(term: Terminal, handler?: (event: MouseEvent, uri: string) => void, options?: ILinkMatcherOptions): void;
export declare function apply(terminalConstructor: typeof Terminal): void;
