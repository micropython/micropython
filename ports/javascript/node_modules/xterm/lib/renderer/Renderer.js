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
var TextRenderLayer_1 = require("./TextRenderLayer");
var SelectionRenderLayer_1 = require("./SelectionRenderLayer");
var CursorRenderLayer_1 = require("./CursorRenderLayer");
var LinkRenderLayer_1 = require("./LinkRenderLayer");
var CharacterJoinerRegistry_1 = require("../renderer/CharacterJoinerRegistry");
var Lifecycle_1 = require("../common/Lifecycle");
var Renderer = (function (_super) {
    __extends(Renderer, _super);
    function Renderer(_terminal, _colors) {
        var _this = _super.call(this) || this;
        _this._terminal = _terminal;
        _this._colors = _colors;
        var allowTransparency = _this._terminal.options.allowTransparency;
        _this._characterJoinerRegistry = new CharacterJoinerRegistry_1.CharacterJoinerRegistry(_terminal);
        _this._renderLayers = [
            new TextRenderLayer_1.TextRenderLayer(_this._terminal.screenElement, 0, _this._colors, _this._characterJoinerRegistry, allowTransparency),
            new SelectionRenderLayer_1.SelectionRenderLayer(_this._terminal.screenElement, 1, _this._colors),
            new LinkRenderLayer_1.LinkRenderLayer(_this._terminal.screenElement, 2, _this._colors, _this._terminal),
            new CursorRenderLayer_1.CursorRenderLayer(_this._terminal.screenElement, 3, _this._colors)
        ];
        _this.dimensions = {
            scaledCharWidth: null,
            scaledCharHeight: null,
            scaledCellWidth: null,
            scaledCellHeight: null,
            scaledCharLeft: null,
            scaledCharTop: null,
            scaledCanvasWidth: null,
            scaledCanvasHeight: null,
            canvasWidth: null,
            canvasHeight: null,
            actualCellWidth: null,
            actualCellHeight: null
        };
        _this._devicePixelRatio = window.devicePixelRatio;
        _this._updateDimensions();
        _this.onOptionsChanged();
        return _this;
    }
    Renderer.prototype.dispose = function () {
        _super.prototype.dispose.call(this);
        this._renderLayers.forEach(function (l) { return l.dispose(); });
    };
    Renderer.prototype.onDevicePixelRatioChange = function () {
        if (this._devicePixelRatio !== window.devicePixelRatio) {
            this._devicePixelRatio = window.devicePixelRatio;
            this.onResize(this._terminal.cols, this._terminal.rows);
        }
    };
    Renderer.prototype.setColors = function (colors) {
        var _this = this;
        this._colors = colors;
        this._renderLayers.forEach(function (l) {
            l.setColors(_this._terminal, _this._colors);
            l.reset(_this._terminal);
        });
    };
    Renderer.prototype.onResize = function (cols, rows) {
        var _this = this;
        this._updateDimensions();
        this._renderLayers.forEach(function (l) { return l.resize(_this._terminal, _this.dimensions); });
        this._terminal.screenElement.style.width = this.dimensions.canvasWidth + "px";
        this._terminal.screenElement.style.height = this.dimensions.canvasHeight + "px";
    };
    Renderer.prototype.onCharSizeChanged = function () {
        this.onResize(this._terminal.cols, this._terminal.rows);
    };
    Renderer.prototype.onBlur = function () {
        var _this = this;
        this._runOperation(function (l) { return l.onBlur(_this._terminal); });
    };
    Renderer.prototype.onFocus = function () {
        var _this = this;
        this._runOperation(function (l) { return l.onFocus(_this._terminal); });
    };
    Renderer.prototype.onSelectionChanged = function (start, end, columnSelectMode) {
        var _this = this;
        if (columnSelectMode === void 0) { columnSelectMode = false; }
        this._runOperation(function (l) { return l.onSelectionChanged(_this._terminal, start, end, columnSelectMode); });
    };
    Renderer.prototype.onCursorMove = function () {
        var _this = this;
        this._runOperation(function (l) { return l.onCursorMove(_this._terminal); });
    };
    Renderer.prototype.onOptionsChanged = function () {
        var _this = this;
        this._runOperation(function (l) { return l.onOptionsChanged(_this._terminal); });
    };
    Renderer.prototype.clear = function () {
        var _this = this;
        this._runOperation(function (l) { return l.reset(_this._terminal); });
    };
    Renderer.prototype._runOperation = function (operation) {
        this._renderLayers.forEach(function (l) { return operation(l); });
    };
    Renderer.prototype.renderRows = function (start, end) {
        var _this = this;
        this._renderLayers.forEach(function (l) { return l.onGridChanged(_this._terminal, start, end); });
    };
    Renderer.prototype._updateDimensions = function () {
        if (!this._terminal.charMeasure.width || !this._terminal.charMeasure.height) {
            return;
        }
        this.dimensions.scaledCharWidth = Math.floor(this._terminal.charMeasure.width * window.devicePixelRatio);
        this.dimensions.scaledCharHeight = Math.ceil(this._terminal.charMeasure.height * window.devicePixelRatio);
        this.dimensions.scaledCellHeight = Math.floor(this.dimensions.scaledCharHeight * this._terminal.options.lineHeight);
        this.dimensions.scaledCharTop = this._terminal.options.lineHeight === 1 ? 0 : Math.round((this.dimensions.scaledCellHeight - this.dimensions.scaledCharHeight) / 2);
        this.dimensions.scaledCellWidth = this.dimensions.scaledCharWidth + Math.round(this._terminal.options.letterSpacing);
        this.dimensions.scaledCharLeft = Math.floor(this._terminal.options.letterSpacing / 2);
        this.dimensions.scaledCanvasHeight = this._terminal.rows * this.dimensions.scaledCellHeight;
        this.dimensions.scaledCanvasWidth = this._terminal.cols * this.dimensions.scaledCellWidth;
        this.dimensions.canvasHeight = Math.round(this.dimensions.scaledCanvasHeight / window.devicePixelRatio);
        this.dimensions.canvasWidth = Math.round(this.dimensions.scaledCanvasWidth / window.devicePixelRatio);
        this.dimensions.actualCellHeight = this.dimensions.canvasHeight / this._terminal.rows;
        this.dimensions.actualCellWidth = this.dimensions.canvasWidth / this._terminal.cols;
    };
    Renderer.prototype.registerCharacterJoiner = function (handler) {
        return this._characterJoinerRegistry.registerCharacterJoiner(handler);
    };
    Renderer.prototype.deregisterCharacterJoiner = function (joinerId) {
        return this._characterJoinerRegistry.deregisterCharacterJoiner(joinerId);
    };
    return Renderer;
}(Lifecycle_1.Disposable));
exports.Renderer = Renderer;
//# sourceMappingURL=Renderer.js.map