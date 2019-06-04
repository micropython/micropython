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
var TextDecoder_1 = require("../input/TextDecoder");
var Types_1 = require("../../common/Types");
exports.DEFAULT_ATTR = (0 << 18) | (Types_1.DEFAULT_COLOR << 9) | (256 << 0);
exports.CHAR_DATA_ATTR_INDEX = 0;
exports.CHAR_DATA_CHAR_INDEX = 1;
exports.CHAR_DATA_WIDTH_INDEX = 2;
exports.CHAR_DATA_CODE_INDEX = 3;
exports.NULL_CELL_CHAR = '';
exports.NULL_CELL_WIDTH = 1;
exports.NULL_CELL_CODE = 0;
exports.WHITESPACE_CELL_CHAR = ' ';
exports.WHITESPACE_CELL_WIDTH = 1;
exports.WHITESPACE_CELL_CODE = 32;
var CELL_SIZE = 3;
var AttributeData = (function () {
    function AttributeData() {
        this.fg = 0;
        this.bg = 0;
    }
    AttributeData.toColorRGB = function (value) {
        return [
            value >>> 16 & 255,
            value >>> 8 & 255,
            value & 255
        ];
    };
    AttributeData.fromColorRGB = function (value) {
        return (value[0] & 255) << 16 | (value[1] & 255) << 8 | value[2] & 255;
    };
    AttributeData.prototype.clone = function () {
        var newObj = new AttributeData();
        newObj.fg = this.fg;
        newObj.bg = this.bg;
        return newObj;
    };
    AttributeData.prototype.isInverse = function () { return this.fg & 67108864; };
    AttributeData.prototype.isBold = function () { return this.fg & 134217728; };
    AttributeData.prototype.isUnderline = function () { return this.fg & 268435456; };
    AttributeData.prototype.isBlink = function () { return this.fg & 536870912; };
    AttributeData.prototype.isInvisible = function () { return this.fg & 1073741824; };
    AttributeData.prototype.isItalic = function () { return this.bg & 67108864; };
    AttributeData.prototype.isDim = function () { return this.bg & 134217728; };
    AttributeData.prototype.getFgColorMode = function () { return this.fg & 50331648; };
    AttributeData.prototype.getBgColorMode = function () { return this.bg & 50331648; };
    AttributeData.prototype.isFgRGB = function () { return (this.fg & 50331648) === 50331648; };
    AttributeData.prototype.isBgRGB = function () { return (this.bg & 50331648) === 50331648; };
    AttributeData.prototype.isFgPalette = function () { return (this.fg & 50331648) === 16777216 || (this.fg & 50331648) === 33554432; };
    AttributeData.prototype.isBgPalette = function () { return (this.bg & 50331648) === 16777216 || (this.bg & 50331648) === 33554432; };
    AttributeData.prototype.isFgDefault = function () { return (this.fg & 50331648) === 0; };
    AttributeData.prototype.isBgDefault = function () { return (this.bg & 50331648) === 0; };
    AttributeData.prototype.getFgColor = function () {
        switch (this.fg & 50331648) {
            case 16777216:
            case 33554432: return this.fg & 255;
            case 50331648: return this.fg & 16777215;
            default: return -1;
        }
    };
    AttributeData.prototype.getBgColor = function () {
        switch (this.bg & 50331648) {
            case 16777216:
            case 33554432: return this.bg & 255;
            case 50331648: return this.bg & 16777215;
            default: return -1;
        }
    };
    return AttributeData;
}());
exports.AttributeData = AttributeData;
exports.DEFAULT_ATTR_DATA = new AttributeData();
var CellData = (function (_super) {
    __extends(CellData, _super);
    function CellData() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this.content = 0;
        _this.fg = 0;
        _this.bg = 0;
        _this.combinedData = '';
        return _this;
    }
    CellData.fromCharData = function (value) {
        var obj = new CellData();
        obj.setFromCharData(value);
        return obj;
    };
    CellData.prototype.isCombined = function () {
        return this.content & 2097152;
    };
    CellData.prototype.getWidth = function () {
        return this.content >> 22;
    };
    CellData.prototype.getChars = function () {
        if (this.content & 2097152) {
            return this.combinedData;
        }
        if (this.content & 2097151) {
            return TextDecoder_1.stringFromCodePoint(this.content & 2097151);
        }
        return '';
    };
    CellData.prototype.getCode = function () {
        return (this.isCombined())
            ? this.combinedData.charCodeAt(this.combinedData.length - 1)
            : this.content & 2097151;
    };
    CellData.prototype.setFromCharData = function (value) {
        this.fg = value[exports.CHAR_DATA_ATTR_INDEX];
        this.bg = 0;
        var combined = false;
        if (value[exports.CHAR_DATA_CHAR_INDEX].length > 2) {
            combined = true;
        }
        else if (value[exports.CHAR_DATA_CHAR_INDEX].length === 2) {
            var code = value[exports.CHAR_DATA_CHAR_INDEX].charCodeAt(0);
            if (0xD800 <= code && code <= 0xDBFF) {
                var second = value[exports.CHAR_DATA_CHAR_INDEX].charCodeAt(1);
                if (0xDC00 <= second && second <= 0xDFFF) {
                    this.content = ((code - 0xD800) * 0x400 + second - 0xDC00 + 0x10000) | (value[exports.CHAR_DATA_WIDTH_INDEX] << 22);
                }
                else {
                    combined = true;
                }
            }
            else {
                combined = true;
            }
        }
        else {
            this.content = value[exports.CHAR_DATA_CHAR_INDEX].charCodeAt(0) | (value[exports.CHAR_DATA_WIDTH_INDEX] << 22);
        }
        if (combined) {
            this.combinedData = value[exports.CHAR_DATA_CHAR_INDEX];
            this.content = 2097152 | (value[exports.CHAR_DATA_WIDTH_INDEX] << 22);
        }
    };
    CellData.prototype.getAsCharData = function () {
        return [this.fg, this.getChars(), this.getWidth(), this.getCode()];
    };
    return CellData;
}(AttributeData));
exports.CellData = CellData;
var BufferLine = (function () {
    function BufferLine(cols, fillCellData, isWrapped) {
        if (isWrapped === void 0) { isWrapped = false; }
        this.isWrapped = isWrapped;
        this._combined = {};
        this._data = new Uint32Array(cols * CELL_SIZE);
        var cell = fillCellData || CellData.fromCharData([0, exports.NULL_CELL_CHAR, exports.NULL_CELL_WIDTH, exports.NULL_CELL_CODE]);
        for (var i = 0; i < cols; ++i) {
            this.setCell(i, cell);
        }
        this.length = cols;
    }
    BufferLine.prototype.get = function (index) {
        var content = this._data[index * CELL_SIZE + 0];
        var cp = content & 2097151;
        return [
            this._data[index * CELL_SIZE + 1],
            (content & 2097152)
                ? this._combined[index]
                : (cp) ? TextDecoder_1.stringFromCodePoint(cp) : '',
            content >> 22,
            (content & 2097152)
                ? this._combined[index].charCodeAt(this._combined[index].length - 1)
                : cp
        ];
    };
    BufferLine.prototype.set = function (index, value) {
        this._data[index * CELL_SIZE + 1] = value[exports.CHAR_DATA_ATTR_INDEX];
        if (value[exports.CHAR_DATA_CHAR_INDEX].length > 1) {
            this._combined[index] = value[1];
            this._data[index * CELL_SIZE + 0] = index | 2097152 | (value[exports.CHAR_DATA_WIDTH_INDEX] << 22);
        }
        else {
            this._data[index * CELL_SIZE + 0] = value[exports.CHAR_DATA_CHAR_INDEX].charCodeAt(0) | (value[exports.CHAR_DATA_WIDTH_INDEX] << 22);
        }
    };
    BufferLine.prototype.getWidth = function (index) {
        return this._data[index * CELL_SIZE + 0] >> 22;
    };
    BufferLine.prototype.hasWidth = function (index) {
        return this._data[index * CELL_SIZE + 0] & 12582912;
    };
    BufferLine.prototype.getFg = function (index) {
        return this._data[index * CELL_SIZE + 1];
    };
    BufferLine.prototype.getBg = function (index) {
        return this._data[index * CELL_SIZE + 2];
    };
    BufferLine.prototype.hasContent = function (index) {
        return this._data[index * CELL_SIZE + 0] & 4194303;
    };
    BufferLine.prototype.getCodePoint = function (index) {
        var content = this._data[index * CELL_SIZE + 0];
        if (content & 2097152) {
            return this._combined[index].charCodeAt(this._combined[index].length - 1);
        }
        return content & 2097151;
    };
    BufferLine.prototype.isCombined = function (index) {
        return this._data[index * CELL_SIZE + 0] & 2097152;
    };
    BufferLine.prototype.getString = function (index) {
        var content = this._data[index * CELL_SIZE + 0];
        if (content & 2097152) {
            return this._combined[index];
        }
        if (content & 2097151) {
            return TextDecoder_1.stringFromCodePoint(content & 2097151);
        }
        return '';
    };
    BufferLine.prototype.loadCell = function (index, cell) {
        var startIndex = index * CELL_SIZE;
        cell.content = this._data[startIndex + 0];
        cell.fg = this._data[startIndex + 1];
        cell.bg = this._data[startIndex + 2];
        if (cell.content & 2097152) {
            cell.combinedData = this._combined[index];
        }
        return cell;
    };
    BufferLine.prototype.setCell = function (index, cell) {
        if (cell.content & 2097152) {
            this._combined[index] = cell.combinedData;
        }
        this._data[index * CELL_SIZE + 0] = cell.content;
        this._data[index * CELL_SIZE + 1] = cell.fg;
        this._data[index * CELL_SIZE + 2] = cell.bg;
    };
    BufferLine.prototype.setCellFromCodePoint = function (index, codePoint, width, fg, bg) {
        this._data[index * CELL_SIZE + 0] = codePoint | (width << 22);
        this._data[index * CELL_SIZE + 1] = fg;
        this._data[index * CELL_SIZE + 2] = bg;
    };
    BufferLine.prototype.addCodepointToCell = function (index, codePoint) {
        var content = this._data[index * CELL_SIZE + 0];
        if (content & 2097152) {
            this._combined[index] += TextDecoder_1.stringFromCodePoint(codePoint);
        }
        else {
            if (content & 2097151) {
                this._combined[index] = TextDecoder_1.stringFromCodePoint(content & 2097151) + TextDecoder_1.stringFromCodePoint(codePoint);
                content &= ~2097151;
                content |= 2097152;
            }
            else {
                content = codePoint | (1 << 22);
            }
            this._data[index * CELL_SIZE + 0] = content;
        }
    };
    BufferLine.prototype.insertCells = function (pos, n, fillCellData) {
        pos %= this.length;
        if (n < this.length - pos) {
            var cell = new CellData();
            for (var i = this.length - pos - n - 1; i >= 0; --i) {
                this.setCell(pos + n + i, this.loadCell(pos + i, cell));
            }
            for (var i = 0; i < n; ++i) {
                this.setCell(pos + i, fillCellData);
            }
        }
        else {
            for (var i = pos; i < this.length; ++i) {
                this.setCell(i, fillCellData);
            }
        }
    };
    BufferLine.prototype.deleteCells = function (pos, n, fillCellData) {
        pos %= this.length;
        if (n < this.length - pos) {
            var cell = new CellData();
            for (var i = 0; i < this.length - pos - n; ++i) {
                this.setCell(pos + i, this.loadCell(pos + n + i, cell));
            }
            for (var i = this.length - n; i < this.length; ++i) {
                this.setCell(i, fillCellData);
            }
        }
        else {
            for (var i = pos; i < this.length; ++i) {
                this.setCell(i, fillCellData);
            }
        }
    };
    BufferLine.prototype.replaceCells = function (start, end, fillCellData) {
        while (start < end && start < this.length) {
            this.setCell(start++, fillCellData);
        }
    };
    BufferLine.prototype.resize = function (cols, fillCellData) {
        if (cols === this.length) {
            return;
        }
        if (cols > this.length) {
            var data = new Uint32Array(cols * CELL_SIZE);
            if (this.length) {
                if (cols * CELL_SIZE < this._data.length) {
                    data.set(this._data.subarray(0, cols * CELL_SIZE));
                }
                else {
                    data.set(this._data);
                }
            }
            this._data = data;
            for (var i = this.length; i < cols; ++i) {
                this.setCell(i, fillCellData);
            }
        }
        else {
            if (cols) {
                var data = new Uint32Array(cols * CELL_SIZE);
                data.set(this._data.subarray(0, cols * CELL_SIZE));
                this._data = data;
                var keys = Object.keys(this._combined);
                for (var i = 0; i < keys.length; i++) {
                    var key = parseInt(keys[i], 10);
                    if (key >= cols) {
                        delete this._combined[key];
                    }
                }
            }
            else {
                this._data = new Uint32Array(0);
                this._combined = {};
            }
        }
        this.length = cols;
    };
    BufferLine.prototype.fill = function (fillCellData) {
        this._combined = {};
        for (var i = 0; i < this.length; ++i) {
            this.setCell(i, fillCellData);
        }
    };
    BufferLine.prototype.copyFrom = function (line) {
        if (this.length !== line.length) {
            this._data = new Uint32Array(line._data);
        }
        else {
            this._data.set(line._data);
        }
        this.length = line.length;
        this._combined = {};
        for (var el in line._combined) {
            this._combined[el] = line._combined[el];
        }
        this.isWrapped = line.isWrapped;
    };
    BufferLine.prototype.clone = function () {
        var newLine = new BufferLine(0);
        newLine._data = new Uint32Array(this._data);
        newLine.length = this.length;
        for (var el in this._combined) {
            newLine._combined[el] = this._combined[el];
        }
        newLine.isWrapped = this.isWrapped;
        return newLine;
    };
    BufferLine.prototype.getTrimmedLength = function () {
        for (var i = this.length - 1; i >= 0; --i) {
            if ((this._data[i * CELL_SIZE + 0] & 4194303)) {
                return i + (this._data[i * CELL_SIZE + 0] >> 22);
            }
        }
        return 0;
    };
    BufferLine.prototype.copyCellsFrom = function (src, srcCol, destCol, length, applyInReverse) {
        var srcData = src._data;
        if (applyInReverse) {
            for (var cell = length - 1; cell >= 0; cell--) {
                for (var i = 0; i < CELL_SIZE; i++) {
                    this._data[(destCol + cell) * CELL_SIZE + i] = srcData[(srcCol + cell) * CELL_SIZE + i];
                }
            }
        }
        else {
            for (var cell = 0; cell < length; cell++) {
                for (var i = 0; i < CELL_SIZE; i++) {
                    this._data[(destCol + cell) * CELL_SIZE + i] = srcData[(srcCol + cell) * CELL_SIZE + i];
                }
            }
        }
        var srcCombinedKeys = Object.keys(src._combined);
        for (var i = 0; i < srcCombinedKeys.length; i++) {
            var key = parseInt(srcCombinedKeys[i], 10);
            if (key >= srcCol) {
                this._combined[key - srcCol + destCol] = src._combined[key];
            }
        }
    };
    BufferLine.prototype.translateToString = function (trimRight, startCol, endCol) {
        if (trimRight === void 0) { trimRight = false; }
        if (startCol === void 0) { startCol = 0; }
        if (endCol === void 0) { endCol = this.length; }
        if (trimRight) {
            endCol = Math.min(endCol, this.getTrimmedLength());
        }
        var result = '';
        while (startCol < endCol) {
            var content = this._data[startCol * CELL_SIZE + 0];
            var cp = content & 2097151;
            result += (content & 2097152) ? this._combined[startCol] : (cp) ? TextDecoder_1.stringFromCodePoint(cp) : exports.WHITESPACE_CELL_CHAR;
            startCol += (content >> 22) || 1;
        }
        return result;
    };
    return BufferLine;
}());
exports.BufferLine = BufferLine;
//# sourceMappingURL=BufferLine.js.map