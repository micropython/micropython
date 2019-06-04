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
var BufferLine_1 = require("../core/buffer/BufferLine");
var JoinedCellData = (function (_super) {
    __extends(JoinedCellData, _super);
    function JoinedCellData(firstCell, chars, width) {
        var _this = _super.call(this) || this;
        _this.content = 0;
        _this.combinedData = '';
        _this.fg = firstCell.fg;
        _this.bg = firstCell.bg;
        _this.combinedData = chars;
        _this._width = width;
        return _this;
    }
    JoinedCellData.prototype.isCombined = function () {
        return 2097152;
    };
    JoinedCellData.prototype.getWidth = function () {
        return this._width;
    };
    JoinedCellData.prototype.getChars = function () {
        return this.combinedData;
    };
    JoinedCellData.prototype.getCode = function () {
        return 0x1FFFFF;
    };
    JoinedCellData.prototype.setFromCharData = function (value) {
        throw new Error('not implemented');
    };
    JoinedCellData.prototype.getAsCharData = function () {
        return [this.fg, this.getChars(), this.getWidth(), this.getCode()];
    };
    return JoinedCellData;
}(BufferLine_1.AttributeData));
exports.JoinedCellData = JoinedCellData;
var CharacterJoinerRegistry = (function () {
    function CharacterJoinerRegistry(_terminal) {
        this._terminal = _terminal;
        this._characterJoiners = [];
        this._nextCharacterJoinerId = 0;
        this._workCell = new BufferLine_1.CellData();
    }
    CharacterJoinerRegistry.prototype.registerCharacterJoiner = function (handler) {
        var joiner = {
            id: this._nextCharacterJoinerId++,
            handler: handler
        };
        this._characterJoiners.push(joiner);
        return joiner.id;
    };
    CharacterJoinerRegistry.prototype.deregisterCharacterJoiner = function (joinerId) {
        for (var i = 0; i < this._characterJoiners.length; i++) {
            if (this._characterJoiners[i].id === joinerId) {
                this._characterJoiners.splice(i, 1);
                return true;
            }
        }
        return false;
    };
    CharacterJoinerRegistry.prototype.getJoinedCharacters = function (row) {
        if (this._characterJoiners.length === 0) {
            return [];
        }
        var line = this._terminal.buffer.lines.get(row);
        if (line.length === 0) {
            return [];
        }
        var ranges = [];
        var lineStr = line.translateToString(true);
        var rangeStartColumn = 0;
        var currentStringIndex = 0;
        var rangeStartStringIndex = 0;
        var rangeAttrFG = line.getFg(0);
        var rangeAttrBG = line.getBg(0);
        for (var x = 0; x < line.getTrimmedLength(); x++) {
            line.loadCell(x, this._workCell);
            if (this._workCell.getWidth() === 0) {
                continue;
            }
            if (this._workCell.fg !== rangeAttrFG || this._workCell.bg !== rangeAttrBG) {
                if (x - rangeStartColumn > 1) {
                    var joinedRanges = this._getJoinedRanges(lineStr, rangeStartStringIndex, currentStringIndex, line, rangeStartColumn);
                    for (var i = 0; i < joinedRanges.length; i++) {
                        ranges.push(joinedRanges[i]);
                    }
                }
                rangeStartColumn = x;
                rangeStartStringIndex = currentStringIndex;
                rangeAttrFG = this._workCell.fg;
                rangeAttrBG = this._workCell.bg;
            }
            currentStringIndex += this._workCell.getChars().length || BufferLine_1.WHITESPACE_CELL_CHAR.length;
        }
        if (this._terminal.cols - rangeStartColumn > 1) {
            var joinedRanges = this._getJoinedRanges(lineStr, rangeStartStringIndex, currentStringIndex, line, rangeStartColumn);
            for (var i = 0; i < joinedRanges.length; i++) {
                ranges.push(joinedRanges[i]);
            }
        }
        return ranges;
    };
    CharacterJoinerRegistry.prototype._getJoinedRanges = function (line, startIndex, endIndex, lineData, startCol) {
        var text = line.substring(startIndex, endIndex);
        var joinedRanges = this._characterJoiners[0].handler(text);
        for (var i = 1; i < this._characterJoiners.length; i++) {
            var joinerRanges = this._characterJoiners[i].handler(text);
            for (var j = 0; j < joinerRanges.length; j++) {
                CharacterJoinerRegistry._mergeRanges(joinedRanges, joinerRanges[j]);
            }
        }
        this._stringRangesToCellRanges(joinedRanges, lineData, startCol);
        return joinedRanges;
    };
    CharacterJoinerRegistry.prototype._stringRangesToCellRanges = function (ranges, line, startCol) {
        var currentRangeIndex = 0;
        var currentRangeStarted = false;
        var currentStringIndex = 0;
        var currentRange = ranges[currentRangeIndex];
        if (!currentRange) {
            return;
        }
        for (var x = startCol; x < this._terminal.cols; x++) {
            var width = line.getWidth(x);
            var length_1 = line.getString(x).length || BufferLine_1.WHITESPACE_CELL_CHAR.length;
            if (width === 0) {
                continue;
            }
            if (!currentRangeStarted && currentRange[0] <= currentStringIndex) {
                currentRange[0] = x;
                currentRangeStarted = true;
            }
            if (currentRange[1] <= currentStringIndex) {
                currentRange[1] = x;
                currentRange = ranges[++currentRangeIndex];
                if (!currentRange) {
                    break;
                }
                if (currentRange[0] <= currentStringIndex) {
                    currentRange[0] = x;
                    currentRangeStarted = true;
                }
                else {
                    currentRangeStarted = false;
                }
            }
            currentStringIndex += length_1;
        }
        if (currentRange) {
            currentRange[1] = this._terminal.cols;
        }
    };
    CharacterJoinerRegistry._mergeRanges = function (ranges, newRange) {
        var inRange = false;
        for (var i = 0; i < ranges.length; i++) {
            var range = ranges[i];
            if (!inRange) {
                if (newRange[1] <= range[0]) {
                    ranges.splice(i, 0, newRange);
                    return ranges;
                }
                if (newRange[1] <= range[1]) {
                    range[0] = Math.min(newRange[0], range[0]);
                    return ranges;
                }
                if (newRange[0] < range[1]) {
                    range[0] = Math.min(newRange[0], range[0]);
                    inRange = true;
                }
                continue;
            }
            else {
                if (newRange[1] <= range[0]) {
                    ranges[i - 1][1] = newRange[1];
                    return ranges;
                }
                if (newRange[1] <= range[1]) {
                    ranges[i - 1][1] = Math.max(newRange[1], range[1]);
                    ranges.splice(i, 1);
                    inRange = false;
                    return ranges;
                }
                ranges.splice(i, 1);
                i--;
            }
        }
        if (inRange) {
            ranges[ranges.length - 1][1] = newRange[1];
        }
        else {
            ranges.push(newRange);
        }
        return ranges;
    };
    return CharacterJoinerRegistry;
}());
exports.CharacterJoinerRegistry = CharacterJoinerRegistry;
//# sourceMappingURL=CharacterJoinerRegistry.js.map