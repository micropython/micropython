import { IColorManager, IColor, IColorSet, ITheme } from './Types';
export declare const DEFAULT_ANSI_COLORS: IColor[];
export declare class ColorManager implements IColorManager {
    allowTransparency: boolean;
    colors: IColorSet;
    private _ctx;
    private _litmusColor;
    constructor(document: Document, allowTransparency: boolean);
    setTheme(theme?: ITheme): void;
    private _parseColor;
}
