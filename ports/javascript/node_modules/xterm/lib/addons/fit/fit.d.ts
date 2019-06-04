import { Terminal } from 'xterm';
export interface IGeometry {
    rows: number;
    cols: number;
}
export declare function proposeGeometry(term: Terminal): IGeometry;
export declare function fit(term: Terminal): void;
export declare function apply(terminalConstructor: typeof Terminal): void;
