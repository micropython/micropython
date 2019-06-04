import { IDisposable } from './Types';
export declare abstract class Disposable implements IDisposable {
    protected _disposables: IDisposable[];
    protected _isDisposed: boolean;
    constructor();
    dispose(): void;
    register<T extends IDisposable>(d: T): void;
    unregister<T extends IDisposable>(d: T): void;
}
