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
var GridCache_1 = require("./GridCache");
var BaseRenderLayer_1 = require("./BaseRenderLayer");
var BufferLine_1 = require("../core/buffer/BufferLine");
var CharacterJoinerRegistry_1 = require("./CharacterJoinerRegistry");
var TextRenderLayer = (function (_super) {
    __extends(TextRenderLayer, _super);
    function TextRenderLayer(container, zIndex, colors, characterJoinerRegistry, alpha) {
        var _this = _super.call(this, container, 'text', zIndex, alpha, colors) || this;
        _this._characterOverlapCache = {};
        _this._workCell = new BufferLine_1.CellData();
        _this._state = new GridCache_1.GridCache();
        _this._characterJoinerRegistry = characterJoinerRegistry;
        return _this;
    }
    TextRenderLayer.prototype.resize = function (terminal, dim) {
        _super.prototype.resize.call(this, terminal, dim);
        var terminalFont = this._getFont(terminal, false, false);
        if (this._characterWidth !== dim.scaledCharWidth || this._characterFont !== terminalFont) {
            this._characterWidth = dim.scaledCharWidth;
            this._characterFont = terminalFont;
            this._characterOverlapCache = {};
        }
        this._state.clear();
        this._state.resize(terminal.cols, terminal.rows);
    };
    TextRenderLayer.prototype.reset = function (terminal) {
        this._state.clear();
        this.clearAll();
    };
    TextRenderLayer.prototype._forEachCell = function (terminal, firstRow, lastRow, joinerRegistry, callback) {
        for (var y = firstRow; y <= lastRow; y++) {
            var row = y + terminal.buffer.ydisp;
            var line = terminal.buffer.lines.get(row);
            var joinedRanges = joinerRegistry ? joinerRegistry.getJoinedCharacters(row) : [];
            for (var x = 0; x < terminal.cols; x++) {
                line.loadCell(x, this._workCell);
                var cell = this._workCell;
                var isJoined = false;
                var lastCharX = x;
                if (cell.getWidth() === 0) {
                    continue;
                }
                if (joinedRanges.length > 0 && x === joinedRanges[0][0]) {
                    isJoined = true;
                    var range = joinedRanges.shift();
                    cell = new CharacterJoinerRegistry_1.JoinedCellData(this._workCell, line.translateToString(true, range[0], range[1]), range[1] - range[0]);
                    lastCharX = range[1] - 1;
                }
                if (!isJoined && this._isOverlapping(cell)) {
                    if (lastCharX < line.length - 1 && line.getCodePoint(lastCharX + 1) === BufferLine_1.NULL_CELL_CODE) {
                        cell.content &= ~12582912;
                        cell.content |= 2 << 22;
                    }
                }
                callback(cell, x, y);
                x = lastCharX;
            }
        }
    };
    TextRenderLayer.prototype._drawBackground = function (terminal, firstRow, lastRow) {
        var _this = this;
        var ctx = this._ctx;
        var cols = terminal.cols;
        var startX = 0;
        var startY = 0;
        var prevFillStyle = null;
        ctx.save();
        this._forEachCell(terminal, firstRow, lastRow, null, function (cell, x, y) {
            var nextFillStyle = null;
            if (cell.isInverse()) {
                if (cell.isFgDefault()) {
                    nextFillStyle = _this._colors.foreground.css;
                }
                else if (cell.isFgRGB()) {
                    nextFillStyle = "rgb(" + BufferLine_1.AttributeData.toColorRGB(cell.getFgColor()).join(',') + ")";
                }
                else {
                    nextFillStyle = _this._colors.ansi[cell.getFgColor()].css;
                }
            }
            else if (cell.isBgRGB()) {
                nextFillStyle = "rgb(" + BufferLine_1.AttributeData.toColorRGB(cell.getBgColor()).join(',') + ")";
            }
            else if (cell.isBgPalette()) {
                nextFillStyle = _this._colors.ansi[cell.getBgColor()].css;
            }
            if (prevFillStyle === null) {
                startX = x;
                startY = y;
            }
            if (y !== startY) {
                ctx.fillStyle = prevFillStyle;
                _this.fillCells(startX, startY, cols - startX, 1);
                startX = x;
                startY = y;
            }
            else if (prevFillStyle !== nextFillStyle) {
                ctx.fillStyle = prevFillStyle;
                _this.fillCells(startX, startY, x - startX, 1);
                startX = x;
                startY = y;
            }
            prevFillStyle = nextFillStyle;
        });
        if (prevFillStyle !== null) {
            ctx.fillStyle = prevFillStyle;
            this.fillCells(startX, startY, cols - startX, 1);
        }
        ctx.restore();
    };
    TextRenderLayer.prototype._drawForeground = function (terminal, firstRow, lastRow) {
        var _this = this;
        this._forEachCell(terminal, firstRow, lastRow, this._characterJoinerRegistry, function (cell, x, y) {
            if (cell.isInvisible()) {
                return;
            }
            _this.drawChars(terminal, cell, x, y);
            if (cell.isUnderline()) {
                _this._ctx.save();
                if (cell.isInverse()) {
                    if (cell.isBgDefault()) {
                        _this._ctx.fillStyle = _this._colors.background.css;
                    }
                    else if (cell.isBgRGB()) {
                        _this._ctx.fillStyle = "rgb(" + BufferLine_1.AttributeData.toColorRGB(cell.getBgColor()).join(',') + ")";
                    }
                    else {
                        _this._ctx.fillStyle = _this._colors.ansi[cell.getBgColor()].css;
                    }
                }
                else {
                    if (cell.isFgDefault()) {
                        _this._ctx.fillStyle = _this._colors.foreground.css;
                    }
                    else if (cell.isFgRGB()) {
                        _this._ctx.fillStyle = "rgb(" + BufferLine_1.AttributeData.toColorRGB(cell.getFgColor()).join(',') + ")";
                    }
                    else {
                        var fg = cell.getFgColor();
                        if (terminal.options.drawBoldTextInBrightColors && cell.isBold() && fg < 8) {
                            fg += 8;
                        }
                        _this._ctx.fillStyle = _this._colors.ansi[fg].css;
                    }
                }
                _this.fillBottomLineAtCells(x, y, cell.getWidth());
                _this._ctx.restore();
            }
        });
    };
    TextRenderLayer.prototype.onGridChanged = function (terminal, firstRow, lastRow) {
        if (this._state.cache.length === 0) {
            return;
        }
        if (this._charAtlas) {
            this._charAtlas.beginFrame();
        }
        this.clearCells(0, firstRow, terminal.cols, lastRow - firstRow + 1);
        this._drawBackground(terminal, firstRow, lastRow);
        this._drawForeground(terminal, firstRow, lastRow);
    };
    TextRenderLayer.prototype.onOptionsChanged = function (terminal) {
        this.setTransparency(terminal, terminal.options.allowTransparency);
    };
    TextRenderLayer.prototype._isOverlapping = function (cell) {
        if (cell.getWidth() !== 1) {
            return false;
        }
        if (cell.getCode() < 256) {
            return false;
        }
        var chars = cell.getChars();
        if (this._characterOverlapCache.hasOwnProperty(chars)) {
            return this._characterOverlapCache[chars];
        }
        this._ctx.save();
        this._ctx.font = this._characterFont;
        var overlaps = Math.floor(this._ctx.measureText(chars).width) > this._characterWidth;
        this._ctx.restore();
        this._characterOverlapCache[chars] = overlaps;
        return overlaps;
    };
    return TextRenderLayer;
}(BaseRenderLayer_1.BaseRenderLayer));
exports.TextRenderLayer = TextRenderLayer;
//# sourceMappingURL=TextRenderLayer.js.map