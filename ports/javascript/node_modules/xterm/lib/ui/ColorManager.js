"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var DEFAULT_FOREGROUND = fromHex('#ffffff');
var DEFAULT_BACKGROUND = fromHex('#000000');
var DEFAULT_CURSOR = fromHex('#ffffff');
var DEFAULT_CURSOR_ACCENT = fromHex('#000000');
var DEFAULT_SELECTION = {
    css: 'rgba(255, 255, 255, 0.3)',
    rgba: 0xFFFFFF77
};
exports.DEFAULT_ANSI_COLORS = (function () {
    var colors = [
        fromHex('#2e3436'),
        fromHex('#cc0000'),
        fromHex('#4e9a06'),
        fromHex('#c4a000'),
        fromHex('#3465a4'),
        fromHex('#75507b'),
        fromHex('#06989a'),
        fromHex('#d3d7cf'),
        fromHex('#555753'),
        fromHex('#ef2929'),
        fromHex('#8ae234'),
        fromHex('#fce94f'),
        fromHex('#729fcf'),
        fromHex('#ad7fa8'),
        fromHex('#34e2e2'),
        fromHex('#eeeeec')
    ];
    var v = [0x00, 0x5f, 0x87, 0xaf, 0xd7, 0xff];
    for (var i = 0; i < 216; i++) {
        var r = v[(i / 36) % 6 | 0];
        var g = v[(i / 6) % 6 | 0];
        var b = v[i % 6];
        colors.push({
            css: "#" + toPaddedHex(r) + toPaddedHex(g) + toPaddedHex(b),
            rgba: ((r << 24) | (g << 16) | (b << 8) | 0xFF) >>> 0
        });
    }
    for (var i = 0; i < 24; i++) {
        var c = 8 + i * 10;
        var ch = toPaddedHex(c);
        colors.push({
            css: "#" + ch + ch + ch,
            rgba: ((c << 24) | (c << 16) | (c << 8) | 0xFF) >>> 0
        });
    }
    return colors;
})();
function fromHex(css) {
    return {
        css: css,
        rgba: parseInt(css.slice(1), 16) << 8 | 0xFF
    };
}
function toPaddedHex(c) {
    var s = c.toString(16);
    return s.length < 2 ? '0' + s : s;
}
var ColorManager = (function () {
    function ColorManager(document, allowTransparency) {
        this.allowTransparency = allowTransparency;
        var canvas = document.createElement('canvas');
        canvas.width = 1;
        canvas.height = 1;
        var ctx = canvas.getContext('2d');
        if (!ctx) {
            throw new Error('Could not get rendering context');
        }
        this._ctx = ctx;
        this._ctx.globalCompositeOperation = 'copy';
        this._litmusColor = this._ctx.createLinearGradient(0, 0, 1, 1);
        this.colors = {
            foreground: DEFAULT_FOREGROUND,
            background: DEFAULT_BACKGROUND,
            cursor: DEFAULT_CURSOR,
            cursorAccent: DEFAULT_CURSOR_ACCENT,
            selection: DEFAULT_SELECTION,
            ansi: exports.DEFAULT_ANSI_COLORS.slice()
        };
    }
    ColorManager.prototype.setTheme = function (theme) {
        if (theme === void 0) { theme = {}; }
        this.colors.foreground = this._parseColor(theme.foreground, DEFAULT_FOREGROUND);
        this.colors.background = this._parseColor(theme.background, DEFAULT_BACKGROUND);
        this.colors.cursor = this._parseColor(theme.cursor, DEFAULT_CURSOR, true);
        this.colors.cursorAccent = this._parseColor(theme.cursorAccent, DEFAULT_CURSOR_ACCENT, true);
        this.colors.selection = this._parseColor(theme.selection, DEFAULT_SELECTION, true);
        this.colors.ansi[0] = this._parseColor(theme.black, exports.DEFAULT_ANSI_COLORS[0]);
        this.colors.ansi[1] = this._parseColor(theme.red, exports.DEFAULT_ANSI_COLORS[1]);
        this.colors.ansi[2] = this._parseColor(theme.green, exports.DEFAULT_ANSI_COLORS[2]);
        this.colors.ansi[3] = this._parseColor(theme.yellow, exports.DEFAULT_ANSI_COLORS[3]);
        this.colors.ansi[4] = this._parseColor(theme.blue, exports.DEFAULT_ANSI_COLORS[4]);
        this.colors.ansi[5] = this._parseColor(theme.magenta, exports.DEFAULT_ANSI_COLORS[5]);
        this.colors.ansi[6] = this._parseColor(theme.cyan, exports.DEFAULT_ANSI_COLORS[6]);
        this.colors.ansi[7] = this._parseColor(theme.white, exports.DEFAULT_ANSI_COLORS[7]);
        this.colors.ansi[8] = this._parseColor(theme.brightBlack, exports.DEFAULT_ANSI_COLORS[8]);
        this.colors.ansi[9] = this._parseColor(theme.brightRed, exports.DEFAULT_ANSI_COLORS[9]);
        this.colors.ansi[10] = this._parseColor(theme.brightGreen, exports.DEFAULT_ANSI_COLORS[10]);
        this.colors.ansi[11] = this._parseColor(theme.brightYellow, exports.DEFAULT_ANSI_COLORS[11]);
        this.colors.ansi[12] = this._parseColor(theme.brightBlue, exports.DEFAULT_ANSI_COLORS[12]);
        this.colors.ansi[13] = this._parseColor(theme.brightMagenta, exports.DEFAULT_ANSI_COLORS[13]);
        this.colors.ansi[14] = this._parseColor(theme.brightCyan, exports.DEFAULT_ANSI_COLORS[14]);
        this.colors.ansi[15] = this._parseColor(theme.brightWhite, exports.DEFAULT_ANSI_COLORS[15]);
    };
    ColorManager.prototype._parseColor = function (css, fallback, allowTransparency) {
        if (allowTransparency === void 0) { allowTransparency = this.allowTransparency; }
        if (css === undefined) {
            return fallback;
        }
        this._ctx.fillStyle = this._litmusColor;
        this._ctx.fillStyle = css;
        if (typeof this._ctx.fillStyle !== 'string') {
            console.warn("Color: " + css + " is invalid using fallback " + fallback.css);
            return fallback;
        }
        this._ctx.fillRect(0, 0, 1, 1);
        var data = this._ctx.getImageData(0, 0, 1, 1).data;
        if (!allowTransparency && data[3] !== 0xFF) {
            console.warn("Color: " + css + " is using transparency, but allowTransparency is false. " +
                ("Using fallback " + fallback.css + "."));
            return fallback;
        }
        return {
            css: css,
            rgba: (data[0] << 24 | data[1] << 16 | data[2] << 8 | data[3]) >>> 0
        };
    };
    return ColorManager;
}());
exports.ColorManager = ColorManager;
//# sourceMappingURL=ColorManager.js.map