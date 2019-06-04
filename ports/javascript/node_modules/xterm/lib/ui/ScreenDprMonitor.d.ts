import { Disposable } from '../common/Lifecycle';
export declare type ScreenDprListener = (newDevicePixelRatio?: number, oldDevicePixelRatio?: number) => void;
export declare class ScreenDprMonitor extends Disposable {
    private _currentDevicePixelRatio;
    private _outerListener;
    private _listener;
    private _resolutionMediaMatchList;
    setListener(listener: ScreenDprListener): void;
    dispose(): void;
    private _updateDpr;
    clearListener(): void;
}
