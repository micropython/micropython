export declare function stringFromCodePoint(codePoint: number): string;
export declare function utf32ToString(data: Uint32Array, start?: number, end?: number): string;
export declare class StringToUtf32 {
    private _interim;
    clear(): void;
    decode(input: string, target: Uint32Array): number;
}
export declare class Utf8ToUtf32 {
    interim: Uint8Array;
    clear(): void;
    decode(input: Uint8Array, target: Uint32Array): number;
}
