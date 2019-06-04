import { IEvent, EventEmitter2 } from './EventEmitter2';
import { IDeleteEvent, IInsertEvent } from './CircularList';
export declare const DEFAULT_COLOR = 256;
export interface IDisposable {
    dispose(): void;
}
export interface IEventEmitter {
    on(type: string, listener: (...args: any[]) => void): void;
    off(type: string, listener: (...args: any[]) => void): void;
    emit(type: string, data?: any): void;
    addDisposableListener(type: string, handler: (...args: any[]) => void): IDisposable;
}
export declare type XtermListener = (...args: any[]) => void;
export interface IKeyboardEvent {
    altKey: boolean;
    ctrlKey: boolean;
    shiftKey: boolean;
    metaKey: boolean;
    keyCode: number;
    key: string;
    type: string;
}
export interface ICircularList<T> {
    length: number;
    maxLength: number;
    isFull: boolean;
    onDeleteEmitter: EventEmitter2<IDeleteEvent>;
    onDelete: IEvent<IDeleteEvent>;
    onInsertEmitter: EventEmitter2<IInsertEvent>;
    onInsert: IEvent<IInsertEvent>;
    onTrimEmitter: EventEmitter2<number>;
    onTrim: IEvent<number>;
    get(index: number): T | undefined;
    set(index: number, value: T): void;
    push(value: T): void;
    recycle(): T | undefined;
    pop(): T | undefined;
    splice(start: number, deleteCount: number, ...items: T[]): void;
    trimStart(count: number): void;
    shiftElements(start: number, count: number, offset: number): void;
}
