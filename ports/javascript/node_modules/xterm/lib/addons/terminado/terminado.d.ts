import { Terminal } from 'xterm';
export declare function terminadoAttach(term: Terminal, socket: WebSocket, bidirectional: boolean, buffered: boolean): void;
export declare function terminadoDetach(term: Terminal, socket: WebSocket): void;
export declare function apply(terminalConstructor: typeof Terminal): void;
