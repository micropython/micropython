import { Terminal, IDisposable } from 'xterm';
export interface ITerminadoAddonTerminal extends Terminal {
    _core: {
        register<T extends IDisposable>(d: T): void;
    };
    __socket?: WebSocket;
    __attachSocketBuffer?: string;
    __dataListener?: IDisposable;
    __getMessage?(ev: MessageEvent): void;
    __flushBuffer?(): void;
    __pushToBuffer?(data: string): void;
    __sendData?(data: string): void;
    __setSize?(size: {
        rows: number;
        cols: number;
    }): void;
}
