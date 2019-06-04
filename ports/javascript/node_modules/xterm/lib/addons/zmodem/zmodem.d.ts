import { Terminal } from 'xterm';
export interface IZmodemOptions {
    noTerminalWriteOutsideSession?: boolean;
}
export declare function apply(terminalConstructor: typeof Terminal): void;
