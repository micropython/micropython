import { ICircularList } from './Types';
import { EventEmitter2, IEvent } from './EventEmitter2';
export interface IInsertEvent {
    index: number;
    amount: number;
}
export interface IDeleteEvent {
    index: number;
    amount: number;
}
export declare class CircularList<T> implements ICircularList<T> {
    private _maxLength;
    protected _array: (T | undefined)[];
    private _startIndex;
    private _length;
    onDeleteEmitter: EventEmitter2<IDeleteEvent>;
    readonly onDelete: IEvent<IDeleteEvent>;
    onInsertEmitter: EventEmitter2<IInsertEvent>;
    readonly onInsert: IEvent<IInsertEvent>;
    onTrimEmitter: EventEmitter2<number>;
    readonly onTrim: IEvent<number>;
    constructor(_maxLength: number);
    maxLength: number;
    length: number;
    get(index: number): T | undefined;
    set(index: number, value: T | undefined): void;
    push(value: T): void;
    recycle(): T;
    readonly isFull: boolean;
    pop(): T | undefined;
    splice(start: number, deleteCount: number, ...items: T[]): void;
    trimStart(count: number): void;
    shiftElements(start: number, count: number, offset: number): void;
    private _getCyclicIndex;
}
