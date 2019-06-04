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
var BaseCharAtlas_1 = require("./BaseCharAtlas");
var ColorManager_1 = require("../../ui/ColorManager");
var CharAtlasGenerator_1 = require("./CharAtlasGenerator");
var LRUMap_1 = require("./LRUMap");
var Platform_1 = require("../../common/Platform");
var TEXTURE_WIDTH = 1024;
var TEXTURE_HEIGHT = 1024;
var TRANSPARENT_COLOR = {
    css: 'rgba(0, 0, 0, 0)',
    rgba: 0
};
var FRAME_CACHE_DRAW_LIMIT = 100;
var GLYPH_BITMAP_COMMIT_DELAY = 100;
function getGlyphCacheKey(glyph) {
    return glyph.code << 21 | glyph.bg << 12 | glyph.fg << 3 | (glyph.bold ? 0 : 4) + (glyph.dim ? 0 : 2) + (glyph.italic ? 0 : 1);
}
exports.getGlyphCacheKey = getGlyphCacheKey;
var DynamicCharAtlas = (function (_super) {
    __extends(DynamicCharAtlas, _super);
    function DynamicCharAtlas(document, _config) {
        var _this = _super.call(this) || this;
        _this._config = _config;
        _this._drawToCacheCount = 0;
        _this._glyphsWaitingOnBitmap = [];
        _this._bitmapCommitTimeout = null;
        _this._bitmap = null;
        _this._cacheCanvas = document.createElement('canvas');
        _this._cacheCanvas.width = TEXTURE_WIDTH;
        _this._cacheCanvas.height = TEXTURE_HEIGHT;
        _this._cacheCtx = _this._cacheCanvas.getContext('2d', { alpha: true });
        var tmpCanvas = document.createElement('canvas');
        tmpCanvas.width = _this._config.scaledCharWidth;
        tmpCanvas.height = _this._config.scaledCharHeight;
        _this._tmpCtx = tmpCanvas.getContext('2d', { alpha: _this._config.allowTransparency });
        _this._width = Math.floor(TEXTURE_WIDTH / _this._config.scaledCharWidth);
        _this._height = Math.floor(TEXTURE_HEIGHT / _this._config.scaledCharHeight);
        var capacity = _this._width * _this._height;
        _this._cacheMap = new LRUMap_1.default(capacity);
        _this._cacheMap.prealloc(capacity);
        return _this;
    }
    DynamicCharAtlas.prototype.dispose = function () {
        if (this._bitmapCommitTimeout !== null) {
            window.clearTimeout(this._bitmapCommitTimeout);
            this._bitmapCommitTimeout = null;
        }
    };
    DynamicCharAtlas.prototype.beginFrame = function () {
        this._drawToCacheCount = 0;
    };
    DynamicCharAtlas.prototype.draw = function (ctx, glyph, x, y) {
        if (glyph.code === 32) {
            return true;
        }
        if (!this._canCache(glyph)) {
            return false;
        }
        var glyphKey = getGlyphCacheKey(glyph);
        var cacheValue = this._cacheMap.get(glyphKey);
        if (cacheValue !== null && cacheValue !== undefined) {
            this._drawFromCache(ctx, cacheValue, x, y);
            return true;
        }
        else if (this._drawToCacheCount < FRAME_CACHE_DRAW_LIMIT) {
            var index = void 0;
            if (this._cacheMap.size < this._cacheMap.capacity) {
                index = this._cacheMap.size;
            }
            else {
                index = this._cacheMap.peek().index;
            }
            var cacheValue_1 = this._drawToCache(glyph, index);
            this._cacheMap.set(glyphKey, cacheValue_1);
            this._drawFromCache(ctx, cacheValue_1, x, y);
            return true;
        }
        return false;
    };
    DynamicCharAtlas.prototype._canCache = function (glyph) {
        return glyph.code < 256;
    };
    DynamicCharAtlas.prototype._toCoordinateX = function (index) {
        return (index % this._width) * this._config.scaledCharWidth;
    };
    DynamicCharAtlas.prototype._toCoordinateY = function (index) {
        return Math.floor(index / this._width) * this._config.scaledCharHeight;
    };
    DynamicCharAtlas.prototype._drawFromCache = function (ctx, cacheValue, x, y) {
        if (cacheValue.isEmpty) {
            return;
        }
        var cacheX = this._toCoordinateX(cacheValue.index);
        var cacheY = this._toCoordinateY(cacheValue.index);
        ctx.drawImage(cacheValue.inBitmap ? this._bitmap : this._cacheCanvas, cacheX, cacheY, this._config.scaledCharWidth, this._config.scaledCharHeight, x, y, this._config.scaledCharWidth, this._config.scaledCharHeight);
    };
    DynamicCharAtlas.prototype._getColorFromAnsiIndex = function (idx) {
        if (idx < this._config.colors.ansi.length) {
            return this._config.colors.ansi[idx];
        }
        return ColorManager_1.DEFAULT_ANSI_COLORS[idx];
    };
    DynamicCharAtlas.prototype._getBackgroundColor = function (glyph) {
        if (this._config.allowTransparency) {
            return TRANSPARENT_COLOR;
        }
        else if (glyph.bg === Types_1.INVERTED_DEFAULT_COLOR) {
            return this._config.colors.foreground;
        }
        else if (glyph.bg < 256) {
            return this._getColorFromAnsiIndex(glyph.bg);
        }
        return this._config.colors.background;
    };
    DynamicCharAtlas.prototype._getForegroundColor = function (glyph) {
        if (glyph.fg === Types_1.INVERTED_DEFAULT_COLOR) {
            return this._config.colors.background;
        }
        else if (glyph.fg < 256) {
            return this._getColorFromAnsiIndex(glyph.fg);
        }
        return this._config.colors.foreground;
    };
    DynamicCharAtlas.prototype._drawToCache = function (glyph, index) {
        this._drawToCacheCount++;
        this._tmpCtx.save();
        var backgroundColor = this._getBackgroundColor(glyph);
        this._tmpCtx.globalCompositeOperation = 'copy';
        this._tmpCtx.fillStyle = backgroundColor.css;
        this._tmpCtx.fillRect(0, 0, this._config.scaledCharWidth, this._config.scaledCharHeight);
        this._tmpCtx.globalCompositeOperation = 'source-over';
        var fontWeight = glyph.bold ? this._config.fontWeightBold : this._config.fontWeight;
        var fontStyle = glyph.italic ? 'italic' : '';
        this._tmpCtx.font =
            fontStyle + " " + fontWeight + " " + this._config.fontSize * this._config.devicePixelRatio + "px " + this._config.fontFamily;
        this._tmpCtx.textBaseline = 'middle';
        this._tmpCtx.fillStyle = this._getForegroundColor(glyph).css;
        if (glyph.dim) {
            this._tmpCtx.globalAlpha = Types_1.DIM_OPACITY;
        }
        this._tmpCtx.fillText(glyph.chars, 0, this._config.scaledCharHeight / 2);
        this._tmpCtx.restore();
        var imageData = this._tmpCtx.getImageData(0, 0, this._config.scaledCharWidth, this._config.scaledCharHeight);
        var isEmpty = false;
        if (!this._config.allowTransparency) {
            isEmpty = CharAtlasGenerator_1.clearColor(imageData, backgroundColor);
        }
        var x = this._toCoordinateX(index);
        var y = this._toCoordinateY(index);
        this._cacheCtx.putImageData(imageData, x, y);
        var cacheValue = {
            index: index,
            isEmpty: isEmpty,
            inBitmap: false
        };
        this._addGlyphToBitmap(cacheValue);
        return cacheValue;
    };
    DynamicCharAtlas.prototype._addGlyphToBitmap = function (cacheValue) {
        var _this = this;
        if (!('createImageBitmap' in window) || Platform_1.isFirefox || Platform_1.isSafari) {
            return;
        }
        this._glyphsWaitingOnBitmap.push(cacheValue);
        if (this._bitmapCommitTimeout !== null) {
            return;
        }
        this._bitmapCommitTimeout = window.setTimeout(function () { return _this._generateBitmap(); }, GLYPH_BITMAP_COMMIT_DELAY);
    };
    DynamicCharAtlas.prototype._generateBitmap = function () {
        var _this = this;
        var glyphsMovingToBitmap = this._glyphsWaitingOnBitmap;
        this._glyphsWaitingOnBitmap = [];
        window.createImageBitmap(this._cacheCanvas).then(function (bitmap) {
            _this._bitmap = bitmap;
            for (var i = 0; i < glyphsMovingToBitmap.length; i++) {
                var value = glyphsMovingToBitmap[i];
                value.inBitmap = true;
            }
        });
        this._bitmapCommitTimeout = null;
    };
    return DynamicCharAtlas;
}(BaseCharAtlas_1.default));
exports.default = DynamicCharAtlas;
//# sourceMappingURL=DynamicCharAtlas.js.map