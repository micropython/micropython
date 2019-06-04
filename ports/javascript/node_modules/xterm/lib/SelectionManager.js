"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var MouseHelper_1 = require("./MouseHelper");
var Browser = require("./common/Platform");
var SelectionModel_1 = require("./SelectionModel");
var AltClickHandler_1 = require("./handlers/AltClickHandler");
var BufferLine_1 = require("./core/buffer/BufferLine");
var EventEmitter2_1 = require("./common/EventEmitter2");
var DRAG_SCROLL_MAX_THRESHOLD = 50;
var DRAG_SCROLL_MAX_SPEED = 15;
var DRAG_SCROLL_INTERVAL = 50;
var ALT_CLICK_MOVE_CURSOR_TIME = 500;
var WORD_SEPARATORS = ' ()[]{}\'"';
var NON_BREAKING_SPACE_CHAR = String.fromCharCode(160);
var ALL_NON_BREAKING_SPACE_REGEX = new RegExp(NON_BREAKING_SPACE_CHAR, 'g');
var SelectionManager = (function () {
    function SelectionManager(_terminal, _charMeasure) {
        this._terminal = _terminal;
        this._charMeasure = _charMeasure;
        this._enabled = true;
        this._workCell = new BufferLine_1.CellData();
        this._onLinuxMouseSelection = new EventEmitter2_1.EventEmitter2();
        this._onRedrawRequest = new EventEmitter2_1.EventEmitter2();
        this._onSelectionChange = new EventEmitter2_1.EventEmitter2();
        this._initListeners();
        this.enable();
        this._model = new SelectionModel_1.SelectionModel(_terminal);
        this._activeSelectionMode = 0;
    }
    Object.defineProperty(SelectionManager.prototype, "onLinuxMouseSelection", {
        get: function () { return this._onLinuxMouseSelection.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SelectionManager.prototype, "onRedrawRequest", {
        get: function () { return this._onRedrawRequest.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SelectionManager.prototype, "onSelectionChange", {
        get: function () { return this._onSelectionChange.event; },
        enumerable: true,
        configurable: true
    });
    SelectionManager.prototype.dispose = function () {
        this._removeMouseDownListeners();
    };
    Object.defineProperty(SelectionManager.prototype, "_buffer", {
        get: function () {
            return this._terminal.buffers.active;
        },
        enumerable: true,
        configurable: true
    });
    SelectionManager.prototype._initListeners = function () {
        var _this = this;
        this._mouseMoveListener = function (event) { return _this._onMouseMove(event); };
        this._mouseUpListener = function (event) { return _this._onMouseUp(event); };
        this.initBuffersListeners();
    };
    SelectionManager.prototype.initBuffersListeners = function () {
        var _this = this;
        this._trimListener = this._terminal.buffer.lines.onTrim(function (amount) { return _this._onTrim(amount); });
        this._terminal.buffers.onBufferActivate(function (e) { return _this._onBufferActivate(e); });
    };
    SelectionManager.prototype.disable = function () {
        this.clearSelection();
        this._enabled = false;
    };
    SelectionManager.prototype.enable = function () {
        this._enabled = true;
    };
    Object.defineProperty(SelectionManager.prototype, "selectionStart", {
        get: function () { return this._model.finalSelectionStart; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SelectionManager.prototype, "selectionEnd", {
        get: function () { return this._model.finalSelectionEnd; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SelectionManager.prototype, "hasSelection", {
        get: function () {
            var start = this._model.finalSelectionStart;
            var end = this._model.finalSelectionEnd;
            if (!start || !end) {
                return false;
            }
            return start[0] !== end[0] || start[1] !== end[1];
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(SelectionManager.prototype, "selectionText", {
        get: function () {
            var start = this._model.finalSelectionStart;
            var end = this._model.finalSelectionEnd;
            if (!start || !end) {
                return '';
            }
            var result = [];
            if (this._activeSelectionMode === 3) {
                if (start[0] === end[0]) {
                    return '';
                }
                for (var i = start[1]; i <= end[1]; i++) {
                    var lineText = this._buffer.translateBufferLineToString(i, true, start[0], end[0]);
                    result.push(lineText);
                }
            }
            else {
                var startRowEndCol = start[1] === end[1] ? end[0] : undefined;
                result.push(this._buffer.translateBufferLineToString(start[1], true, start[0], startRowEndCol));
                for (var i = start[1] + 1; i <= end[1] - 1; i++) {
                    var bufferLine = this._buffer.lines.get(i);
                    var lineText = this._buffer.translateBufferLineToString(i, true);
                    if (bufferLine.isWrapped) {
                        result[result.length - 1] += lineText;
                    }
                    else {
                        result.push(lineText);
                    }
                }
                if (start[1] !== end[1]) {
                    var bufferLine = this._buffer.lines.get(end[1]);
                    var lineText = this._buffer.translateBufferLineToString(end[1], true, 0, end[0]);
                    if (bufferLine.isWrapped) {
                        result[result.length - 1] += lineText;
                    }
                    else {
                        result.push(lineText);
                    }
                }
            }
            var formattedResult = result.map(function (line) {
                return line.replace(ALL_NON_BREAKING_SPACE_REGEX, ' ');
            }).join(Browser.isMSWindows ? '\r\n' : '\n');
            return formattedResult;
        },
        enumerable: true,
        configurable: true
    });
    SelectionManager.prototype.clearSelection = function () {
        this._model.clearSelection();
        this._removeMouseDownListeners();
        this.refresh();
        this._onSelectionChange.fire();
    };
    SelectionManager.prototype.refresh = function (isLinuxMouseSelection) {
        var _this = this;
        if (!this._refreshAnimationFrame) {
            this._refreshAnimationFrame = window.requestAnimationFrame(function () { return _this._refresh(); });
        }
        if (Browser.isLinux && isLinuxMouseSelection) {
            var selectionText = this.selectionText;
            if (selectionText.length) {
                this._onLinuxMouseSelection.fire(this.selectionText);
            }
        }
    };
    SelectionManager.prototype._refresh = function () {
        this._refreshAnimationFrame = null;
        this._onRedrawRequest.fire({
            start: this._model.finalSelectionStart,
            end: this._model.finalSelectionEnd,
            columnSelectMode: this._activeSelectionMode === 3
        });
    };
    SelectionManager.prototype.isClickInSelection = function (event) {
        var coords = this._getMouseBufferCoords(event);
        var start = this._model.finalSelectionStart;
        var end = this._model.finalSelectionEnd;
        if (!start || !end) {
            return false;
        }
        return this._areCoordsInSelection(coords, start, end);
    };
    SelectionManager.prototype._areCoordsInSelection = function (coords, start, end) {
        return (coords[1] > start[1] && coords[1] < end[1]) ||
            (start[1] === end[1] && coords[1] === start[1] && coords[0] >= start[0] && coords[0] < end[0]) ||
            (start[1] < end[1] && coords[1] === end[1] && coords[0] < end[0]) ||
            (start[1] < end[1] && coords[1] === start[1] && coords[0] >= start[0]);
    };
    SelectionManager.prototype.selectWordAtCursor = function (event) {
        var coords = this._getMouseBufferCoords(event);
        if (coords) {
            this._selectWordAt(coords, false);
            this._model.selectionEnd = null;
            this.refresh(true);
        }
    };
    SelectionManager.prototype.selectAll = function () {
        this._model.isSelectAllActive = true;
        this.refresh();
        this._onSelectionChange.fire();
    };
    SelectionManager.prototype.selectLines = function (start, end) {
        this._model.clearSelection();
        start = Math.max(start, 0);
        end = Math.min(end, this._terminal.buffer.lines.length - 1);
        this._model.selectionStart = [0, start];
        this._model.selectionEnd = [this._terminal.cols, end];
        this.refresh();
        this._onSelectionChange.fire();
    };
    SelectionManager.prototype._onTrim = function (amount) {
        var needsRefresh = this._model.onTrim(amount);
        if (needsRefresh) {
            this.refresh();
        }
    };
    SelectionManager.prototype._getMouseBufferCoords = function (event) {
        var coords = this._terminal.mouseHelper.getCoords(event, this._terminal.screenElement, this._charMeasure, this._terminal.cols, this._terminal.rows, true);
        if (!coords) {
            return null;
        }
        coords[0]--;
        coords[1]--;
        coords[1] += this._terminal.buffer.ydisp;
        return coords;
    };
    SelectionManager.prototype._getMouseEventScrollAmount = function (event) {
        var offset = MouseHelper_1.MouseHelper.getCoordsRelativeToElement(event, this._terminal.screenElement)[1];
        var terminalHeight = this._terminal.rows * Math.ceil(this._charMeasure.height * this._terminal.options.lineHeight);
        if (offset >= 0 && offset <= terminalHeight) {
            return 0;
        }
        if (offset > terminalHeight) {
            offset -= terminalHeight;
        }
        offset = Math.min(Math.max(offset, -DRAG_SCROLL_MAX_THRESHOLD), DRAG_SCROLL_MAX_THRESHOLD);
        offset /= DRAG_SCROLL_MAX_THRESHOLD;
        return (offset / Math.abs(offset)) + Math.round(offset * (DRAG_SCROLL_MAX_SPEED - 1));
    };
    SelectionManager.prototype.shouldForceSelection = function (event) {
        if (Browser.isMac) {
            return event.altKey && this._terminal.options.macOptionClickForcesSelection;
        }
        return event.shiftKey;
    };
    SelectionManager.prototype.onMouseDown = function (event) {
        this._mouseDownTimeStamp = event.timeStamp;
        if (event.button === 2 && this.hasSelection) {
            return;
        }
        if (event.button !== 0) {
            return;
        }
        if (!this._enabled) {
            if (!this.shouldForceSelection(event)) {
                return;
            }
            event.stopPropagation();
        }
        event.preventDefault();
        this._dragScrollAmount = 0;
        if (this._enabled && event.shiftKey) {
            this._onIncrementalClick(event);
        }
        else {
            if (event.detail === 1) {
                this._onSingleClick(event);
            }
            else if (event.detail === 2) {
                this._onDoubleClick(event);
            }
            else if (event.detail === 3) {
                this._onTripleClick(event);
            }
        }
        this._addMouseDownListeners();
        this.refresh(true);
    };
    SelectionManager.prototype._addMouseDownListeners = function () {
        var _this = this;
        this._terminal.element.ownerDocument.addEventListener('mousemove', this._mouseMoveListener);
        this._terminal.element.ownerDocument.addEventListener('mouseup', this._mouseUpListener);
        this._dragScrollIntervalTimer = setInterval(function () { return _this._dragScroll(); }, DRAG_SCROLL_INTERVAL);
    };
    SelectionManager.prototype._removeMouseDownListeners = function () {
        if (this._terminal.element.ownerDocument) {
            this._terminal.element.ownerDocument.removeEventListener('mousemove', this._mouseMoveListener);
            this._terminal.element.ownerDocument.removeEventListener('mouseup', this._mouseUpListener);
        }
        clearInterval(this._dragScrollIntervalTimer);
        this._dragScrollIntervalTimer = null;
    };
    SelectionManager.prototype._onIncrementalClick = function (event) {
        if (this._model.selectionStart) {
            this._model.selectionEnd = this._getMouseBufferCoords(event);
        }
    };
    SelectionManager.prototype._onSingleClick = function (event) {
        this._model.selectionStartLength = 0;
        this._model.isSelectAllActive = false;
        this._activeSelectionMode = this.shouldColumnSelect(event) ? 3 : 0;
        this._model.selectionStart = this._getMouseBufferCoords(event);
        if (!this._model.selectionStart) {
            return;
        }
        this._model.selectionEnd = null;
        var line = this._buffer.lines.get(this._model.selectionStart[1]);
        if (!line) {
            return;
        }
        if (line.length >= this._model.selectionStart[0]) {
            return;
        }
        if (line.hasWidth(this._model.selectionStart[0]) === 0) {
            this._model.selectionStart[0]++;
        }
    };
    SelectionManager.prototype._onDoubleClick = function (event) {
        var coords = this._getMouseBufferCoords(event);
        if (coords) {
            this._activeSelectionMode = 1;
            this._selectWordAt(coords, true);
        }
    };
    SelectionManager.prototype._onTripleClick = function (event) {
        var coords = this._getMouseBufferCoords(event);
        if (coords) {
            this._activeSelectionMode = 2;
            this._selectLineAt(coords[1]);
        }
    };
    SelectionManager.prototype.shouldColumnSelect = function (event) {
        return event.altKey && !(Browser.isMac && this._terminal.options.macOptionClickForcesSelection);
    };
    SelectionManager.prototype._onMouseMove = function (event) {
        event.stopImmediatePropagation();
        var previousSelectionEnd = this._model.selectionEnd ? [this._model.selectionEnd[0], this._model.selectionEnd[1]] : null;
        this._model.selectionEnd = this._getMouseBufferCoords(event);
        if (!this._model.selectionEnd) {
            this.refresh(true);
            return;
        }
        if (this._activeSelectionMode === 2) {
            if (this._model.selectionEnd[1] < this._model.selectionStart[1]) {
                this._model.selectionEnd[0] = 0;
            }
            else {
                this._model.selectionEnd[0] = this._terminal.cols;
            }
        }
        else if (this._activeSelectionMode === 1) {
            this._selectToWordAt(this._model.selectionEnd);
        }
        this._dragScrollAmount = this._getMouseEventScrollAmount(event);
        if (this._activeSelectionMode !== 3) {
            if (this._dragScrollAmount > 0) {
                this._model.selectionEnd[0] = this._terminal.cols;
            }
            else if (this._dragScrollAmount < 0) {
                this._model.selectionEnd[0] = 0;
            }
        }
        if (this._model.selectionEnd[1] < this._buffer.lines.length) {
            if (this._buffer.lines.get(this._model.selectionEnd[1]).hasWidth(this._model.selectionEnd[0]) === 0) {
                this._model.selectionEnd[0]++;
            }
        }
        if (!previousSelectionEnd ||
            previousSelectionEnd[0] !== this._model.selectionEnd[0] ||
            previousSelectionEnd[1] !== this._model.selectionEnd[1]) {
            this.refresh(true);
        }
    };
    SelectionManager.prototype._dragScroll = function () {
        if (this._dragScrollAmount) {
            this._terminal.scrollLines(this._dragScrollAmount, false);
            if (this._dragScrollAmount > 0) {
                if (this._activeSelectionMode !== 3) {
                    this._model.selectionEnd[0] = this._terminal.cols;
                }
                this._model.selectionEnd[1] = Math.min(this._terminal.buffer.ydisp + this._terminal.rows, this._terminal.buffer.lines.length - 1);
            }
            else {
                if (this._activeSelectionMode !== 3) {
                    this._model.selectionEnd[0] = 0;
                }
                this._model.selectionEnd[1] = this._terminal.buffer.ydisp;
            }
            this.refresh();
        }
    };
    SelectionManager.prototype._onMouseUp = function (event) {
        var timeElapsed = event.timeStamp - this._mouseDownTimeStamp;
        this._removeMouseDownListeners();
        if (this.selectionText.length <= 1 && timeElapsed < ALT_CLICK_MOVE_CURSOR_TIME) {
            (new AltClickHandler_1.AltClickHandler(event, this._terminal)).move();
        }
        else if (this.hasSelection) {
            this._onSelectionChange.fire();
        }
    };
    SelectionManager.prototype._onBufferActivate = function (e) {
        var _this = this;
        this.clearSelection();
        if (this._trimListener) {
            this._trimListener.dispose();
        }
        this._trimListener = e.activeBuffer.lines.onTrim(function (amount) { return _this._onTrim(amount); });
    };
    SelectionManager.prototype._convertViewportColToCharacterIndex = function (bufferLine, coords) {
        var charIndex = coords[0];
        for (var i = 0; coords[0] >= i; i++) {
            var length_1 = bufferLine.loadCell(i, this._workCell).getChars().length;
            if (this._workCell.getWidth() === 0) {
                charIndex--;
            }
            else if (length_1 > 1 && coords[0] !== i) {
                charIndex += length_1 - 1;
            }
        }
        return charIndex;
    };
    SelectionManager.prototype.setSelection = function (col, row, length) {
        this._model.clearSelection();
        this._removeMouseDownListeners();
        this._model.selectionStart = [col, row];
        this._model.selectionStartLength = length;
        this.refresh();
    };
    SelectionManager.prototype._getWordAt = function (coords, allowWhitespaceOnlySelection, followWrappedLinesAbove, followWrappedLinesBelow) {
        if (followWrappedLinesAbove === void 0) { followWrappedLinesAbove = true; }
        if (followWrappedLinesBelow === void 0) { followWrappedLinesBelow = true; }
        if (coords[0] >= this._terminal.cols) {
            return null;
        }
        var bufferLine = this._buffer.lines.get(coords[1]);
        if (!bufferLine) {
            return null;
        }
        var line = this._buffer.translateBufferLineToString(coords[1], false);
        var startIndex = this._convertViewportColToCharacterIndex(bufferLine, coords);
        var endIndex = startIndex;
        var charOffset = coords[0] - startIndex;
        var leftWideCharCount = 0;
        var rightWideCharCount = 0;
        var leftLongCharOffset = 0;
        var rightLongCharOffset = 0;
        if (line.charAt(startIndex) === ' ') {
            while (startIndex > 0 && line.charAt(startIndex - 1) === ' ') {
                startIndex--;
            }
            while (endIndex < line.length && line.charAt(endIndex + 1) === ' ') {
                endIndex++;
            }
        }
        else {
            var startCol = coords[0];
            var endCol = coords[0];
            if (bufferLine.getWidth(startCol) === 0) {
                leftWideCharCount++;
                startCol--;
            }
            if (bufferLine.getWidth(endCol) === 2) {
                rightWideCharCount++;
                endCol++;
            }
            var length_2 = bufferLine.getString(endCol).length;
            if (length_2 > 1) {
                rightLongCharOffset += length_2 - 1;
                endIndex += length_2 - 1;
            }
            while (startCol > 0 && startIndex > 0 && !this._isCharWordSeparator(bufferLine.loadCell(startCol - 1, this._workCell))) {
                bufferLine.loadCell(startCol - 1, this._workCell);
                var length_3 = this._workCell.getChars().length;
                if (this._workCell.getWidth() === 0) {
                    leftWideCharCount++;
                    startCol--;
                }
                else if (length_3 > 1) {
                    leftLongCharOffset += length_3 - 1;
                    startIndex -= length_3 - 1;
                }
                startIndex--;
                startCol--;
            }
            while (endCol < bufferLine.length && endIndex + 1 < line.length && !this._isCharWordSeparator(bufferLine.loadCell(endCol + 1, this._workCell))) {
                bufferLine.loadCell(endCol + 1, this._workCell);
                var length_4 = this._workCell.getChars().length;
                if (this._workCell.getWidth() === 2) {
                    rightWideCharCount++;
                    endCol++;
                }
                else if (length_4 > 1) {
                    rightLongCharOffset += length_4 - 1;
                    endIndex += length_4 - 1;
                }
                endIndex++;
                endCol++;
            }
        }
        endIndex++;
        var start = startIndex
            + charOffset
            - leftWideCharCount
            + leftLongCharOffset;
        var length = Math.min(this._terminal.cols, endIndex
            - startIndex
            + leftWideCharCount
            + rightWideCharCount
            - leftLongCharOffset
            - rightLongCharOffset);
        if (!allowWhitespaceOnlySelection && line.slice(startIndex, endIndex).trim() === '') {
            return null;
        }
        if (followWrappedLinesAbove) {
            if (start === 0 && bufferLine.getCodePoint(0) !== 32) {
                var previousBufferLine = this._buffer.lines.get(coords[1] - 1);
                if (previousBufferLine && bufferLine.isWrapped && previousBufferLine.getCodePoint(this._terminal.cols - 1) !== 32) {
                    var previousLineWordPosition = this._getWordAt([this._terminal.cols - 1, coords[1] - 1], false, true, false);
                    if (previousLineWordPosition) {
                        var offset = this._terminal.cols - previousLineWordPosition.start;
                        start -= offset;
                        length += offset;
                    }
                }
            }
        }
        if (followWrappedLinesBelow) {
            if (start + length === this._terminal.cols && bufferLine.getCodePoint(this._terminal.cols - 1) !== 32) {
                var nextBufferLine = this._buffer.lines.get(coords[1] + 1);
                if (nextBufferLine && nextBufferLine.isWrapped && nextBufferLine.getCodePoint(0) !== 32) {
                    var nextLineWordPosition = this._getWordAt([0, coords[1] + 1], false, false, true);
                    if (nextLineWordPosition) {
                        length += nextLineWordPosition.length;
                    }
                }
            }
        }
        return { start: start, length: length };
    };
    SelectionManager.prototype._selectWordAt = function (coords, allowWhitespaceOnlySelection) {
        var wordPosition = this._getWordAt(coords, allowWhitespaceOnlySelection);
        if (wordPosition) {
            while (wordPosition.start < 0) {
                wordPosition.start += this._terminal.cols;
                coords[1]--;
            }
            this._model.selectionStart = [wordPosition.start, coords[1]];
            this._model.selectionStartLength = wordPosition.length;
        }
    };
    SelectionManager.prototype._selectToWordAt = function (coords) {
        var wordPosition = this._getWordAt(coords, true);
        if (wordPosition) {
            var endRow = coords[1];
            while (wordPosition.start < 0) {
                wordPosition.start += this._terminal.cols;
                endRow--;
            }
            if (!this._model.areSelectionValuesReversed()) {
                while (wordPosition.start + wordPosition.length > this._terminal.cols) {
                    wordPosition.length -= this._terminal.cols;
                    endRow++;
                }
            }
            this._model.selectionEnd = [this._model.areSelectionValuesReversed() ? wordPosition.start : wordPosition.start + wordPosition.length, endRow];
        }
    };
    SelectionManager.prototype._isCharWordSeparator = function (cell) {
        if (cell.getWidth() === 0) {
            return false;
        }
        return WORD_SEPARATORS.indexOf(cell.getChars()) >= 0;
    };
    SelectionManager.prototype._selectLineAt = function (line) {
        var wrappedRange = this._buffer.getWrappedRangeForLine(line);
        this._model.selectionStart = [0, wrappedRange.first];
        this._model.selectionEnd = [this._terminal.cols, wrappedRange.last];
        this._model.selectionStartLength = 0;
    };
    return SelectionManager;
}());
exports.SelectionManager = SelectionManager;
//# sourceMappingURL=SelectionManager.js.map