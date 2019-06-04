import { IEvent } from '../../common/EventEmitter2';
import { Disposable } from '../../common/Lifecycle';
import { IMarker } from '../Types';
export declare class Marker extends Disposable implements IMarker {
    line: number;
    private static _nextId;
    private _id;
    isDisposed: boolean;
    readonly id: number;
    private _onDispose;
    readonly onDispose: IEvent<void>;
    constructor(line: number);
    dispose(): void;
}
