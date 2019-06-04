import { Terminal } from 'xterm';
export declare function attach(term: Terminal, socket: WebSocket, bidirectional: boolean, buffered: boolean): void;
export declare function detach(term: Terminal, socket: WebSocket): void;
export declare function apply(terminalConstructor: typeof Terminal): void;
