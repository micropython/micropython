export interface IColorManager {
    colors: IColorSet;
}
export interface IColor {
    css: string;
    rgba: number;
}
export interface IColorSet {
    foreground: IColor;
    background: IColor;
    cursor: IColor;
    cursorAccent: IColor;
    selection: IColor;
    ansi: IColor[];
}
export interface ITheme {
    foreground?: string;
    background?: string;
    cursor?: string;
    cursorAccent?: string;
    selection?: string;
    black?: string;
    red?: string;
    green?: string;
    yellow?: string;
    blue?: string;
    magenta?: string;
    cyan?: string;
    white?: string;
    brightBlack?: string;
    brightRed?: string;
    brightGreen?: string;
    brightYellow?: string;
    brightBlue?: string;
    brightMagenta?: string;
    brightCyan?: string;
    brightWhite?: string;
}
