import { IDisposable } from '../common/Types';
export declare class RenderDebouncer implements IDisposable {
    private _renderCallback;
    private _rowStart;
    private _rowEnd;
    private _rowCount;
    private _animationFrame;
    constructor(_renderCallback: (start: number, end: number) => void);
    dispose(): void;
    refresh(rowStart: number, rowEnd: number, rowCount: number): void;
    private _innerRefresh;
}
