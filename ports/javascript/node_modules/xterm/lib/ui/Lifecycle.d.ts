import { IDisposable } from '../common/Types';
export declare function addDisposableDomListener(node: Element | Window | Document, type: string, handler: (e: any) => void, useCapture?: boolean): IDisposable;
