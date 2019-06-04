"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var EscapeSequences_1 = require("../common/data/EscapeSequences");
var AltClickHandler = (function () {
    function AltClickHandler(_mouseEvent, _terminal) {
        var _a;
        this._mouseEvent = _mouseEvent;
        this._terminal = _terminal;
        this._lines = this._terminal.buffer.lines;
        this._startCol = this._terminal.buffer.x;
        this._startRow = this._terminal.buffer.y;
        var coordinates = this._terminal.mouseHelper.getCoords(this._mouseEvent, this._terminal.element, this._terminal.charMeasure, this._terminal.cols, this._terminal.rows, false);
        if (coordinates) {
            _a = coordinates.map(function (coordinate) {
                return coordinate - 1;
            }), this._endCol = _a[0], this._endRow = _a[1];
        }
    }
    AltClickHandler.prototype.move = function () {
        if (this._mouseEvent.altKey && this._endCol !== undefined && this._endRow !== undefined) {
            this._terminal.handler(this._arrowSequences());
        }
    };
    AltClickHandler.prototype._arrowSequences = function () {
        if (!this._terminal.buffer.hasScrollback) {
            return this._resetStartingRow() + this._moveToRequestedRow() + this._moveToRequestedCol();
        }
        return this._moveHorizontallyOnly();
    };
    AltClickHandler.prototype._resetStartingRow = function () {
        if (this._moveToRequestedRow().length === 0) {
            return '';
        }
        return repeat(this._bufferLine(this._startCol, this._startRow, this._startCol, this._startRow - this._wrappedRowsForRow(this._startRow), false).length, this._sequence("D"));
    };
    AltClickHandler.prototype._moveToRequestedRow = function () {
        var startRow = this._startRow - this._wrappedRowsForRow(this._startRow);
        var endRow = this._endRow - this._wrappedRowsForRow(this._endRow);
        var rowsToMove = Math.abs(startRow - endRow) - this._wrappedRowsCount();
        return repeat(rowsToMove, this._sequence(this._verticalDirection()));
    };
    AltClickHandler.prototype._moveToRequestedCol = function () {
        var startRow;
        if (this._moveToRequestedRow().length > 0) {
            startRow = this._endRow - this._wrappedRowsForRow(this._endRow);
        }
        else {
            startRow = this._startRow;
        }
        var endRow = this._endRow;
        var direction = this._horizontalDirection();
        return repeat(this._bufferLine(this._startCol, startRow, this._endCol, endRow, direction === "C").length, this._sequence(direction));
    };
    AltClickHandler.prototype._moveHorizontallyOnly = function () {
        var direction = this._horizontalDirection();
        return repeat(Math.abs(this._startCol - this._endCol), this._sequence(direction));
    };
    AltClickHandler.prototype._wrappedRowsCount = function () {
        var wrappedRows = 0;
        var startRow = this._startRow - this._wrappedRowsForRow(this._startRow);
        var endRow = this._endRow - this._wrappedRowsForRow(this._endRow);
        for (var i = 0; i < Math.abs(startRow - endRow); i++) {
            var direction = this._verticalDirection() === "A" ? -1 : 1;
            if (this._lines.get(startRow + (direction * i)).isWrapped) {
                wrappedRows++;
            }
        }
        return wrappedRows;
    };
    AltClickHandler.prototype._wrappedRowsForRow = function (currentRow) {
        var rowCount = 0;
        var lineWraps = this._lines.get(currentRow).isWrapped;
        while (lineWraps && currentRow >= 0 && currentRow < this._terminal.rows) {
            rowCount++;
            currentRow--;
            lineWraps = this._lines.get(currentRow).isWrapped;
        }
        return rowCount;
    };
    AltClickHandler.prototype._horizontalDirection = function () {
        var startRow;
        if (this._moveToRequestedRow().length > 0) {
            startRow = this._endRow - this._wrappedRowsForRow(this._endRow);
        }
        else {
            startRow = this._startRow;
        }
        if ((this._startCol < this._endCol &&
            startRow <= this._endRow) ||
            (this._startCol >= this._endCol &&
                startRow < this._endRow)) {
            return "C";
        }
        return "D";
    };
    AltClickHandler.prototype._verticalDirection = function () {
        if (this._startRow > this._endRow) {
            return "A";
        }
        return "B";
    };
    AltClickHandler.prototype._bufferLine = function (startCol, startRow, endCol, endRow, forward) {
        var currentCol = startCol;
        var currentRow = startRow;
        var bufferStr = '';
        while (currentCol !== endCol || currentRow !== endRow) {
            currentCol += forward ? 1 : -1;
            if (forward && currentCol > this._terminal.cols - 1) {
                bufferStr += this._terminal.buffer.translateBufferLineToString(currentRow, false, startCol, currentCol);
                currentCol = 0;
                startCol = 0;
                currentRow++;
            }
            else if (!forward && currentCol < 0) {
                bufferStr += this._terminal.buffer.translateBufferLineToString(currentRow, false, 0, startCol + 1);
                currentCol = this._terminal.cols - 1;
                startCol = currentCol;
                currentRow--;
            }
        }
        return bufferStr + this._terminal.buffer.translateBufferLineToString(currentRow, false, startCol, currentCol);
    };
    AltClickHandler.prototype._sequence = function (direction) {
        var mod = this._terminal.applicationCursor ? 'O' : '[';
        return EscapeSequences_1.C0.ESC + mod + direction;
    };
    return AltClickHandler;
}());
exports.AltClickHandler = AltClickHandler;
function repeat(count, str) {
    count = Math.floor(count);
    var rpt = '';
    for (var i = 0; i < count; i++) {
        rpt += str;
    }
    return rpt;
}
//# sourceMappingURL=AltClickHandler.js.map