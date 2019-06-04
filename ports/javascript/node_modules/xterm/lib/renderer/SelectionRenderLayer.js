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
var BaseRenderLayer_1 = require("./BaseRenderLayer");
var SelectionRenderLayer = (function (_super) {
    __extends(SelectionRenderLayer, _super);
    function SelectionRenderLayer(container, zIndex, colors) {
        var _this = _super.call(this, container, 'selection', zIndex, true, colors) || this;
        _this._clearState();
        return _this;
    }
    SelectionRenderLayer.prototype._clearState = function () {
        this._state = {
            start: null,
            end: null,
            columnSelectMode: null,
            ydisp: null
        };
    };
    SelectionRenderLayer.prototype.resize = function (terminal, dim) {
        _super.prototype.resize.call(this, terminal, dim);
        this._clearState();
    };
    SelectionRenderLayer.prototype.reset = function (terminal) {
        if (this._state.start && this._state.end) {
            this._clearState();
            this.clearAll();
        }
    };
    SelectionRenderLayer.prototype.onSelectionChanged = function (terminal, start, end, columnSelectMode) {
        if (!this._didStateChange(start, end, columnSelectMode, terminal.buffer.ydisp)) {
            return;
        }
        this.clearAll();
        if (!start || !end) {
            this._clearState();
            return;
        }
        var viewportStartRow = start[1] - terminal.buffer.ydisp;
        var viewportEndRow = end[1] - terminal.buffer.ydisp;
        var viewportCappedStartRow = Math.max(viewportStartRow, 0);
        var viewportCappedEndRow = Math.min(viewportEndRow, terminal.rows - 1);
        if (viewportCappedStartRow >= terminal.rows || viewportCappedEndRow < 0) {
            return;
        }
        this._ctx.fillStyle = this._colors.selection.css;
        if (columnSelectMode) {
            var startCol = start[0];
            var width = end[0] - startCol;
            var height = viewportCappedEndRow - viewportCappedStartRow + 1;
            this.fillCells(startCol, viewportCappedStartRow, width, height);
        }
        else {
            var startCol = viewportStartRow === viewportCappedStartRow ? start[0] : 0;
            var startRowEndCol = viewportCappedStartRow === viewportCappedEndRow ? end[0] : terminal.cols;
            this.fillCells(startCol, viewportCappedStartRow, startRowEndCol - startCol, 1);
            var middleRowsCount = Math.max(viewportCappedEndRow - viewportCappedStartRow - 1, 0);
            this.fillCells(0, viewportCappedStartRow + 1, terminal.cols, middleRowsCount);
            if (viewportCappedStartRow !== viewportCappedEndRow) {
                var endCol = viewportEndRow === viewportCappedEndRow ? end[0] : terminal.cols;
                this.fillCells(0, viewportCappedEndRow, endCol, 1);
            }
        }
        this._state.start = [start[0], start[1]];
        this._state.end = [end[0], end[1]];
        this._state.columnSelectMode = columnSelectMode;
        this._state.ydisp = terminal.buffer.ydisp;
    };
    SelectionRenderLayer.prototype._didStateChange = function (start, end, columnSelectMode, ydisp) {
        return !this._areCoordinatesEqual(start, this._state.start) ||
            !this._areCoordinatesEqual(end, this._state.end) ||
            columnSelectMode !== this._state.columnSelectMode ||
            ydisp !== this._state.ydisp;
    };
    SelectionRenderLayer.prototype._areCoordinatesEqual = function (coord1, coord2) {
        if (!coord1 || !coord2) {
            return false;
        }
        return coord1[0] === coord2[0] && coord1[1] === coord2[1];
    };
    return SelectionRenderLayer;
}(BaseRenderLayer_1.BaseRenderLayer));
exports.SelectionRenderLayer = SelectionRenderLayer;
//# sourceMappingURL=SelectionRenderLayer.js.map