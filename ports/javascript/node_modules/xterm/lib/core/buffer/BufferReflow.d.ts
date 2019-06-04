import { BufferLine } from './BufferLine';
import { CircularList } from '../../common/CircularList';
import { IBufferLine, ICellData } from '../Types';
export interface INewLayoutResult {
    layout: number[];
    countRemoved: number;
}
export declare function reflowLargerGetLinesToRemove(lines: CircularList<IBufferLine>, oldCols: number, newCols: number, bufferAbsoluteY: number, nullCell: ICellData): number[];
export declare function reflowLargerCreateNewLayout(lines: CircularList<IBufferLine>, toRemove: number[]): INewLayoutResult;
export declare function reflowLargerApplyNewLayout(lines: CircularList<IBufferLine>, newLayout: number[]): void;
export declare function reflowSmallerGetNewLineLengths(wrappedLines: BufferLine[], oldCols: number, newCols: number): number[];
export declare function getWrappedLineTrimmedLength(lines: BufferLine[], i: number, cols: number): number;
