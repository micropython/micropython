"use strict";
var __extends = (this && this.__extends) || (function () {
    var extendStatics = function (d, b) {
        extendStatics = Object.setPrototypeOf ||
            ({ __proto__: [] } instanceof Array && function (d, b) { d.__proto__ = b; }) ||
            function (d, b) { for (var p in b) if (b.hasOwnProperty(p)) d[p] = b[p]; };
        return extendStatics(d, b);
    };
    return function (d, b) {
        extendStatics(d, b);
        function __() { this.constructor = d; }
        d.prototype = b === null ? Object.create(b) : (__.prototype = b.prototype, new __());
    };
})();
Object.defineProperty(exports, "__esModule", { value: true });
var Types_1 = require("./Types");
var CharAtlasGenerator_1 = require("./CharAtlasGenerator");
var BaseCharAtlas_1 = require("./BaseCharAtlas");
var CharAtlasUtils_1 = require("./CharAtlasUtils");
var Types_2 = require("../../common/Types");
var StaticCharAtlas = (function (_super) {
    __extends(StaticCharAtlas, _super);
    function StaticCharAtlas(_document, _config) {
        var _this = _super.call(this) || this;
        _this._document = _document;
        _this._config = _config;
        _this._canvasFactory = function (width, height) {
            var canvas = _this._document.createElement('canvas');
            canvas.width = width;
            canvas.height = height;
            return canvas;
        };
        return _this;
    }
    StaticCharAtlas.prototype._doWarmUp = function () {
        var _this = this;
        var result = CharAtlasGenerator_1.generateStaticCharAtlasTexture(window, this._canvasFactory, this._config);
        if (result instanceof HTMLCanvasElement) {
            this._texture = result;
        }
        else {
            result.then(function (texture) {
                _this._texture = texture;
            });
        }
    };
    StaticCharAtlas.prototype._isCached = function (glyph, colorIndex) {
        var isAscii = glyph.code < 256;
        var isBasicColor = glyph.fg < 16;
        var isDefaultColor = glyph.fg === Types_2.DEFAULT_COLOR;
        var isDefaultBackground = glyph.bg === Types_2.DEFAULT_COLOR;
        return isAscii && (isBasicColor || isDefaultColor) && isDefaultBackground && !glyph.italic;
    };
    StaticCharAtlas.prototype.draw = function (ctx, glyph, x, y) {
        if (this._texture === null || this._texture === undefined) {
            return false;
        }
        var colorIndex = 0;
        if (CharAtlasUtils_1.is256Color(glyph.fg)) {
            colorIndex = 2 + glyph.fg + (glyph.bold ? 16 : 0);
        }
        else if (glyph.fg === Types_2.DEFAULT_COLOR) {
            if (glyph.bold) {
                colorIndex = 1;
            }
        }
        if (!this._isCached(glyph, colorIndex)) {
            return false;
        }
        ctx.save();
        var charAtlasCellWidth = this._config.scaledCharWidth + Types_1.CHAR_ATLAS_CELL_SPACING;
        var charAtlasCellHeight = this._config.scaledCharHeight + Types_1.CHAR_ATLAS_CELL_SPACING;
        if (glyph.dim) {
            ctx.globalAlpha = Types_1.DIM_OPACITY;
        }
        ctx.drawImage(this._texture, glyph.code * charAtlasCellWidth, colorIndex * charAtlasCellHeight, charAtlasCellWidth, this._config.scaledCharHeight, x, y, charAtlasCellWidth, this._config.scaledCharHeight);
        ctx.restore();
        return true;
    };
    return StaticCharAtlas;
}(BaseCharAtlas_1.default));
exports.default = StaticCharAtlas;
//# sourceMappingURL=StaticCharAtlas.js.map