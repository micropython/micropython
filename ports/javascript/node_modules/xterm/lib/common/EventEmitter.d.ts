import { IDisposable, IEventEmitter, XtermListener } from './Types';
import { Disposable } from './Lifecycle';
export declare class EventEmitter extends Disposable implements IEventEmitter, IDisposable {
    private _events;
    constructor();
    on(type: string, listener: XtermListener): void;
    addDisposableListener(type: string, handler: XtermListener): IDisposable;
    off(type: string, listener: XtermListener): void;
    removeAllListeners(type: string): void;
    emit(type: string, ...args: any[]): void;
    emitMayRemoveListeners(type: string, ...args: any[]): void;
    listeners(type: string): XtermListener[];
    dispose(): void;
}
