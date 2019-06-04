export declare type TypedArray = Uint8Array | Uint16Array | Uint32Array | Uint8ClampedArray | Int8Array | Int16Array | Int32Array | Float32Array | Float64Array;
export declare function fill<T extends TypedArray>(array: T, value: number, start?: number, end?: number): T;
export declare function fillFallback<T extends TypedArray>(array: T, value: number, start?: number, end?: number): T;
export declare function concat<T extends TypedArray>(a: T, b: T): T;
