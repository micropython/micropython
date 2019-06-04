"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var CircularList_1 = require("./common/CircularList");
var BufferLine_1 = require("./core/buffer/BufferLine");
var BufferReflow_1 = require("./core/buffer/BufferReflow");
var Marker_1 = require("./core/buffer/Marker");
exports.MAX_BUFFER_SIZE = 4294967295;
var Buffer = (function () {
    function Buffer(_terminal, _hasScrollback) {
        this._terminal = _terminal;
        this._hasScrollback = _hasScrollback;
        this.savedCurAttrData = BufferLine_1.DEFAULT_ATTR_DATA.clone();
        this.markers = [];
        this._nullCell = BufferLine_1.CellData.fromCharData([0, BufferLine_1.NULL_CELL_CHAR, BufferLine_1.NULL_CELL_WIDTH, BufferLine_1.NULL_CELL_CODE]);
        this._whitespaceCell = BufferLine_1.CellData.fromCharData([0, BufferLine_1.WHITESPACE_CELL_CHAR, BufferLine_1.WHITESPACE_CELL_WIDTH, BufferLine_1.WHITESPACE_CELL_CODE]);
        this._cols = this._terminal.cols;
        this._rows = this._terminal.rows;
        this.clear();
    }
    Buffer.prototype.getNullCell = function (attr) {
        if (attr) {
            this._nullCell.fg = attr.fg;
            this._nullCell.bg = attr.bg;
        }
        else {
            this._nullCell.fg = 0;
            this._nullCell.bg = 0;
        }
        return this._nullCell;
    };
    Buffer.prototype.getWhitespaceCell = function (attr) {
        if (attr) {
            this._whitespaceCell.fg = attr.fg;
            this._whitespaceCell.bg = attr.bg;
        }
        else {
            this._whitespaceCell.fg = 0;
            this._whitespaceCell.bg = 0;
        }
        return this._whitespaceCell;
    };
    Buffer.prototype.getBlankLine = function (attr, isWrapped) {
        return new BufferLine_1.BufferLine(this._terminal.cols, this.getNullCell(attr), isWrapped);
    };
    Object.defineProperty(Buffer.prototype, "hasScrollback", {
        get: function () {
            return this._hasScrollback && this.lines.maxLength > this._rows;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Buffer.prototype, "isCursorInViewport", {
        get: function () {
            var absoluteY = this.ybase + this.y;
            var relativeY = absoluteY - this.ydisp;
            return (relativeY >= 0 && relativeY < this._rows);
        },
        enumerable: true,
        configurable: true
    });
    Buffer.prototype._getCorrectBufferLength = function (rows) {
        if (!this._hasScrollback) {
            return rows;
        }
        var correctBufferLength = rows + this._terminal.options.scrollback;
        return correctBufferLength > exports.MAX_BUFFER_SIZE ? exports.MAX_BUFFER_SIZE : correctBufferLength;
    };
    Buffer.prototype.fillViewportRows = function (fillAttr) {
        if (this.lines.length === 0) {
            if (fillAttr === undefined) {
                fillAttr = BufferLine_1.DEFAULT_ATTR_DATA;
            }
            var i = this._rows;
            while (i--) {
                this.lines.push(this.getBlankLine(fillAttr));
            }
        }
    };
    Buffer.prototype.clear = function () {
        this.ydisp = 0;
        this.ybase = 0;
        this.y = 0;
        this.x = 0;
        this.lines = new CircularList_1.CircularList(this._getCorrectBufferLength(this._rows));
        this.scrollTop = 0;
        this.scrollBottom = this._rows - 1;
        this.setupTabStops();
    };
    Buffer.prototype.resize = function (newCols, newRows) {
        var nullCell = this.getNullCell(BufferLine_1.DEFAULT_ATTR_DATA);
        var newMaxLength = this._getCorrectBufferLength(newRows);
        if (newMaxLength > this.lines.maxLength) {
            this.lines.maxLength = newMaxLength;
        }
        if (this.lines.length > 0) {
            if (this._cols < newCols) {
                for (var i = 0; i < this.lines.length; i++) {
                    this.lines.get(i).resize(newCols, nullCell);
                }
            }
            var addToY = 0;
            if (this._rows < newRows) {
                for (var y = this._rows; y < newRows; y++) {
                    if (this.lines.length < newRows + this.ybase) {
                        if (this.ybase > 0 && this.lines.length <= this.ybase + this.y + addToY + 1) {
                            this.ybase--;
                            addToY++;
                            if (this.ydisp > 0) {
                                this.ydisp--;
                            }
                        }
                        else {
                            this.lines.push(new BufferLine_1.BufferLine(newCols, nullCell));
                        }
                    }
                }
            }
            else {
                for (var y = this._rows; y > newRows; y--) {
                    if (this.lines.length > newRows + this.ybase) {
                        if (this.lines.length > this.ybase + this.y + 1) {
                            this.lines.pop();
                        }
                        else {
                            this.ybase++;
                            this.ydisp++;
                        }
                    }
                }
            }
            if (newMaxLength < this.lines.maxLength) {
                var amountToTrim = this.lines.length - newMaxLength;
                if (amountToTrim > 0) {
                    this.lines.trimStart(amountToTrim);
                    this.ybase = Math.max(this.ybase - amountToTrim, 0);
                    this.ydisp = Math.max(this.ydisp - amountToTrim, 0);
                }
                this.lines.maxLength = newMaxLength;
            }
            this.x = Math.min(this.x, newCols - 1);
            this.y = Math.min(this.y, newRows - 1);
            if (addToY) {
                this.y += addToY;
            }
            this.savedY = Math.min(this.savedY, newRows - 1);
            this.savedX = Math.min(this.savedX, newCols - 1);
            this.scrollTop = 0;
        }
        this.scrollBottom = newRows - 1;
        if (this._isReflowEnabled) {
            this._reflow(newCols, newRows);
            if (this._cols > newCols) {
                for (var i = 0; i < this.lines.length; i++) {
                    this.lines.get(i).resize(newCols, nullCell);
                }
            }
        }
        this._cols = newCols;
        this._rows = newRows;
    };
    Object.defineProperty(Buffer.prototype, "_isReflowEnabled", {
        get: function () {
            return this._hasScrollback && !this._terminal.options.windowsMode;
        },
        enumerable: true,
        configurable: true
    });
    Buffer.prototype._reflow = function (newCols, newRows) {
        if (this._cols === newCols) {
            return;
        }
        if (newCols > this._cols) {
            this._reflowLarger(newCols, newRows);
        }
        else {
            this._reflowSmaller(newCols, newRows);
        }
    };
    Buffer.prototype._reflowLarger = function (newCols, newRows) {
        var toRemove = BufferReflow_1.reflowLargerGetLinesToRemove(this.lines, this._cols, newCols, this.ybase + this.y, this.getNullCell(BufferLine_1.DEFAULT_ATTR_DATA));
        if (toRemove.length > 0) {
            var newLayoutResult = BufferReflow_1.reflowLargerCreateNewLayout(this.lines, toRemove);
            BufferReflow_1.reflowLargerApplyNewLayout(this.lines, newLayoutResult.layout);
            this._reflowLargerAdjustViewport(newCols, newRows, newLayoutResult.countRemoved);
        }
    };
    Buffer.prototype._reflowLargerAdjustViewport = function (newCols, newRows, countRemoved) {
        var nullCell = this.getNullCell(BufferLine_1.DEFAULT_ATTR_DATA);
        var viewportAdjustments = countRemoved;
        while (viewportAdjustments-- > 0) {
            if (this.ybase === 0) {
                if (this.y > 0) {
                    this.y--;
                }
                if (this.lines.length < newRows) {
                    this.lines.push(new BufferLine_1.BufferLine(newCols, nullCell));
                }
            }
            else {
                if (this.ydisp === this.ybase) {
                    this.ydisp--;
                }
                this.ybase--;
            }
        }
    };
    Buffer.prototype._reflowSmaller = function (newCols, newRows) {
        var nullCell = this.getNullCell(BufferLine_1.DEFAULT_ATTR_DATA);
        var toInsert = [];
        var countToInsert = 0;
        for (var y = this.lines.length - 1; y >= 0; y--) {
            var nextLine = this.lines.get(y);
            if (!nextLine || !nextLine.isWrapped && nextLine.getTrimmedLength() <= newCols) {
                continue;
            }
            var wrappedLines = [nextLine];
            while (nextLine.isWrapped && y > 0) {
                nextLine = this.lines.get(--y);
                wrappedLines.unshift(nextLine);
            }
            var absoluteY = this.ybase + this.y;
            if (absoluteY >= y && absoluteY < y + wrappedLines.length) {
                continue;
            }
            var lastLineLength = wrappedLines[wrappedLines.length - 1].getTrimmedLength();
            var destLineLengths = BufferReflow_1.reflowSmallerGetNewLineLengths(wrappedLines, this._cols, newCols);
            var linesToAdd = destLineLengths.length - wrappedLines.length;
            var trimmedLines = void 0;
            if (this.ybase === 0 && this.y !== this.lines.length - 1) {
                trimmedLines = Math.max(0, this.y - this.lines.maxLength + linesToAdd);
            }
            else {
                trimmedLines = Math.max(0, this.lines.length - this.lines.maxLength + linesToAdd);
            }
            var newLines = [];
            for (var i = 0; i < linesToAdd; i++) {
                var newLine = this.getBlankLine(BufferLine_1.DEFAULT_ATTR_DATA, true);
                newLines.push(newLine);
            }
            if (newLines.length > 0) {
                toInsert.push({
                    start: y + wrappedLines.length + countToInsert,
                    newLines: newLines
                });
                countToInsert += newLines.length;
            }
            wrappedLines.push.apply(wrappedLines, newLines);
            var destLineIndex = destLineLengths.length - 1;
            var destCol = destLineLengths[destLineIndex];
            if (destCol === 0) {
                destLineIndex--;
                destCol = destLineLengths[destLineIndex];
            }
            var srcLineIndex = wrappedLines.length - linesToAdd - 1;
            var srcCol = lastLineLength;
            while (srcLineIndex >= 0) {
                var cellsToCopy = Math.min(srcCol, destCol);
                wrappedLines[destLineIndex].copyCellsFrom(wrappedLines[srcLineIndex], srcCol - cellsToCopy, destCol - cellsToCopy, cellsToCopy, true);
                destCol -= cellsToCopy;
                if (destCol === 0) {
                    destLineIndex--;
                    destCol = destLineLengths[destLineIndex];
                }
                srcCol -= cellsToCopy;
                if (srcCol === 0) {
                    srcLineIndex--;
                    var wrappedLinesIndex = Math.max(srcLineIndex, 0);
                    srcCol = BufferReflow_1.getWrappedLineTrimmedLength(wrappedLines, wrappedLinesIndex, this._cols);
                }
            }
            for (var i = 0; i < wrappedLines.length; i++) {
                if (destLineLengths[i] < newCols) {
                    wrappedLines[i].setCell(destLineLengths[i], nullCell);
                }
            }
            var viewportAdjustments = linesToAdd - trimmedLines;
            while (viewportAdjustments-- > 0) {
                if (this.ybase === 0) {
                    if (this.y < newRows - 1) {
                        this.y++;
                        this.lines.pop();
                    }
                    else {
                        this.ybase++;
                        this.ydisp++;
                    }
                }
                else {
                    if (this.ybase < Math.min(this.lines.maxLength, this.lines.length + countToInsert) - newRows) {
                        if (this.ybase === this.ydisp) {
                            this.ydisp++;
                        }
                        this.ybase++;
                    }
                }
            }
        }
        if (toInsert.length > 0) {
            var insertEvents = [];
            var originalLines = [];
            for (var i = 0; i < this.lines.length; i++) {
                originalLines.push(this.lines.get(i));
            }
            var originalLinesLength = this.lines.length;
            var originalLineIndex = originalLinesLength - 1;
            var nextToInsertIndex = 0;
            var nextToInsert = toInsert[nextToInsertIndex];
            this.lines.length = Math.min(this.lines.maxLength, this.lines.length + countToInsert);
            var countInsertedSoFar = 0;
            for (var i = Math.min(this.lines.maxLength - 1, originalLinesLength + countToInsert - 1); i >= 0; i--) {
                if (nextToInsert && nextToInsert.start > originalLineIndex + countInsertedSoFar) {
                    for (var nextI = nextToInsert.newLines.length - 1; nextI >= 0; nextI--) {
                        this.lines.set(i--, nextToInsert.newLines[nextI]);
                    }
                    i++;
                    insertEvents.push({
                        index: originalLineIndex + 1,
                        amount: nextToInsert.newLines.length
                    });
                    countInsertedSoFar += nextToInsert.newLines.length;
                    nextToInsert = toInsert[++nextToInsertIndex];
                }
                else {
                    this.lines.set(i, originalLines[originalLineIndex--]);
                }
            }
            var insertCountEmitted = 0;
            for (var i = insertEvents.length - 1; i >= 0; i--) {
                insertEvents[i].index += insertCountEmitted;
                this.lines.onInsertEmitter.fire(insertEvents[i]);
                insertCountEmitted += insertEvents[i].amount;
            }
            var amountToTrim = Math.max(0, originalLinesLength + countToInsert - this.lines.maxLength);
            if (amountToTrim > 0) {
                this.lines.onTrimEmitter.fire(amountToTrim);
            }
        }
    };
    Buffer.prototype.stringIndexToBufferIndex = function (lineIndex, stringIndex, trimRight) {
        if (trimRight === void 0) { trimRight = false; }
        while (stringIndex) {
            var line = this.lines.get(lineIndex);
            if (!line) {
                return [-1, -1];
            }
            var length_1 = (trimRight) ? line.getTrimmedLength() : line.length;
            for (var i = 0; i < length_1; ++i) {
                if (line.get(i)[BufferLine_1.CHAR_DATA_WIDTH_INDEX]) {
                    stringIndex -= line.get(i)[BufferLine_1.CHAR_DATA_CHAR_INDEX].length || 1;
                }
                if (stringIndex < 0) {
                    return [lineIndex, i];
                }
            }
            lineIndex++;
        }
        return [lineIndex, 0];
    };
    Buffer.prototype.translateBufferLineToString = function (lineIndex, trimRight, startCol, endCol) {
        if (startCol === void 0) { startCol = 0; }
        var line = this.lines.get(lineIndex);
        if (!line) {
            return '';
        }
        return line.translateToString(trimRight, startCol, endCol);
    };
    Buffer.prototype.getWrappedRangeForLine = function (y) {
        var first = y;
        var last = y;
        while (first > 0 && this.lines.get(first).isWrapped) {
            first--;
        }
        while (last + 1 < this.lines.length && this.lines.get(last + 1).isWrapped) {
            last++;
        }
        return { first: first, last: last };
    };
    Buffer.prototype.setupTabStops = function (i) {
        if (i !== null && i !== undefined) {
            if (!this.tabs[i]) {
                i = this.prevStop(i);
            }
        }
        else {
            this.tabs = {};
            i = 0;
        }
        for (; i < this._cols; i += this._terminal.options.tabStopWidth) {
            this.tabs[i] = true;
        }
    };
    Buffer.prototype.prevStop = function (x) {
        if (x === null || x === undefined) {
            x = this.x;
        }
        while (!this.tabs[--x] && x > 0)
            ;
        return x >= this._cols ? this._cols - 1 : x < 0 ? 0 : x;
    };
    Buffer.prototype.nextStop = function (x) {
        if (x === null || x === undefined) {
            x = this.x;
        }
        while (!this.tabs[++x] && x < this._cols)
            ;
        return x >= this._cols ? this._cols - 1 : x < 0 ? 0 : x;
    };
    Buffer.prototype.addMarker = function (y) {
        var _this = this;
        var marker = new Marker_1.Marker(y);
        this.markers.push(marker);
        marker.register(this.lines.onTrim(function (amount) {
            marker.line -= amount;
            if (marker.line < 0) {
                marker.dispose();
            }
        }));
        marker.register(this.lines.onInsert(function (event) {
            if (marker.line >= event.index) {
                marker.line += event.amount;
            }
        }));
        marker.register(this.lines.onDelete(function (event) {
            if (marker.line >= event.index && marker.line < event.index + event.amount) {
                marker.dispose();
            }
            if (marker.line > event.index) {
                marker.line -= event.amount;
            }
        }));
        marker.register(marker.onDispose(function () { return _this._removeMarker(marker); }));
        return marker;
    };
    Buffer.prototype._removeMarker = function (marker) {
        this.markers.splice(this.markers.indexOf(marker), 1);
    };
    Buffer.prototype.iterator = function (trimRight, startIndex, endIndex, startOverscan, endOverscan) {
        return new BufferStringIterator(this, trimRight, startIndex, endIndex, startOverscan, endOverscan);
    };
    return Buffer;
}());
exports.Buffer = Buffer;
var BufferStringIterator = (function () {
    function BufferStringIterator(_buffer, _trimRight, _startIndex, _endIndex, _startOverscan, _endOverscan) {
        if (_startIndex === void 0) { _startIndex = 0; }
        if (_endIndex === void 0) { _endIndex = _buffer.lines.length; }
        if (_startOverscan === void 0) { _startOverscan = 0; }
        if (_endOverscan === void 0) { _endOverscan = 0; }
        this._buffer = _buffer;
        this._trimRight = _trimRight;
        this._startIndex = _startIndex;
        this._endIndex = _endIndex;
        this._startOverscan = _startOverscan;
        this._endOverscan = _endOverscan;
        if (this._startIndex < 0) {
            this._startIndex = 0;
        }
        if (this._endIndex > this._buffer.lines.length) {
            this._endIndex = this._buffer.lines.length;
        }
        this._current = this._startIndex;
    }
    BufferStringIterator.prototype.hasNext = function () {
        return this._current < this._endIndex;
    };
    BufferStringIterator.prototype.next = function () {
        var range = this._buffer.getWrappedRangeForLine(this._current);
        if (range.first < this._startIndex - this._startOverscan) {
            range.first = this._startIndex - this._startOverscan;
        }
        if (range.last > this._endIndex + this._endOverscan) {
            range.last = this._endIndex + this._endOverscan;
        }
        range.first = Math.max(range.first, 0);
        range.last = Math.min(range.last, this._buffer.lines.length);
        var result = '';
        for (var i = range.first; i <= range.last; ++i) {
            result += this._buffer.translateBufferLineToString(i, this._trimRight);
        }
        this._current = range.last + 1;
        return { range: range, content: result };
    };
    return BufferStringIterator;
}());
exports.BufferStringIterator = BufferStringIterator;
//# sourceMappingURL=Buffer.js.map