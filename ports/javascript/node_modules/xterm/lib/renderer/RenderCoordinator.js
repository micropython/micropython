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
var RenderDebouncer_1 = require("../ui/RenderDebouncer");
var EventEmitter2_1 = require("../common/EventEmitter2");
var Lifecycle_1 = require("../common/Lifecycle");
var ScreenDprMonitor_1 = require("../ui/ScreenDprMonitor");
var Lifecycle_2 = require("../ui/Lifecycle");
var RenderCoordinator = (function (_super) {
    __extends(RenderCoordinator, _super);
    function RenderCoordinator(_renderer, _rowCount, screenElement) {
        var _this = _super.call(this) || this;
        _this._renderer = _renderer;
        _this._rowCount = _rowCount;
        _this._isPaused = false;
        _this._needsFullRefresh = false;
        _this._canvasWidth = 0;
        _this._canvasHeight = 0;
        _this._onDimensionsChange = new EventEmitter2_1.EventEmitter2();
        _this._onRender = new EventEmitter2_1.EventEmitter2();
        _this._onRefreshRequest = new EventEmitter2_1.EventEmitter2();
        _this._renderDebouncer = new RenderDebouncer_1.RenderDebouncer(function (start, end) { return _this._renderRows(start, end); });
        _this.register(_this._renderDebouncer);
        _this._screenDprMonitor = new ScreenDprMonitor_1.ScreenDprMonitor();
        _this._screenDprMonitor.setListener(function () { return _this._renderer.onDevicePixelRatioChange(); });
        _this.register(_this._screenDprMonitor);
        _this.register(Lifecycle_2.addDisposableDomListener(window, 'resize', function () { return _this._renderer.onDevicePixelRatioChange(); }));
        if ('IntersectionObserver' in window) {
            var observer_1 = new IntersectionObserver(function (e) { return _this._onIntersectionChange(e[e.length - 1]); }, { threshold: 0 });
            observer_1.observe(screenElement);
            _this.register({ dispose: function () { return observer_1.disconnect(); } });
        }
        return _this;
    }
    Object.defineProperty(RenderCoordinator.prototype, "onDimensionsChange", {
        get: function () { return this._onDimensionsChange.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RenderCoordinator.prototype, "onRender", {
        get: function () { return this._onRender.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RenderCoordinator.prototype, "onRefreshRequest", {
        get: function () { return this._onRefreshRequest.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(RenderCoordinator.prototype, "dimensions", {
        get: function () { return this._renderer.dimensions; },
        enumerable: true,
        configurable: true
    });
    RenderCoordinator.prototype._onIntersectionChange = function (entry) {
        this._isPaused = entry.intersectionRatio === 0;
        if (!this._isPaused && this._needsFullRefresh) {
            this.refreshRows(0, this._rowCount - 1);
            this._needsFullRefresh = false;
        }
    };
    RenderCoordinator.prototype.refreshRows = function (start, end) {
        if (this._isPaused) {
            this._needsFullRefresh = true;
            return;
        }
        this._renderDebouncer.refresh(start, end, this._rowCount);
    };
    RenderCoordinator.prototype._renderRows = function (start, end) {
        this._renderer.renderRows(start, end);
        this._onRender.fire({ start: start, end: end });
    };
    RenderCoordinator.prototype.resize = function (cols, rows) {
        this._rowCount = rows;
        this._fireOnCanvasResize();
    };
    RenderCoordinator.prototype.changeOptions = function () {
        this._renderer.onOptionsChanged();
        this._fireOnCanvasResize();
    };
    RenderCoordinator.prototype._fireOnCanvasResize = function () {
        if (this._renderer.dimensions.canvasWidth === this._canvasWidth && this._renderer.dimensions.canvasHeight === this._canvasHeight) {
            return;
        }
        this._onDimensionsChange.fire(this._renderer.dimensions);
    };
    RenderCoordinator.prototype.setRenderer = function (renderer) {
        this._renderer.dispose();
        this._renderer = renderer;
    };
    RenderCoordinator.prototype._fullRefresh = function () {
        if (this._isPaused) {
            this._needsFullRefresh = true;
        }
        else {
            this.refreshRows(0, this._rowCount);
        }
    };
    RenderCoordinator.prototype.setColors = function (colors) {
        this._renderer.setColors(colors);
        this._fullRefresh();
    };
    RenderCoordinator.prototype.onDevicePixelRatioChange = function () {
        this._renderer.onDevicePixelRatioChange();
    };
    RenderCoordinator.prototype.onResize = function (cols, rows) {
        this._renderer.onResize(cols, rows);
        this._fullRefresh();
    };
    RenderCoordinator.prototype.onCharSizeChanged = function () {
        this._renderer.onCharSizeChanged();
    };
    RenderCoordinator.prototype.onBlur = function () {
        this._renderer.onBlur();
    };
    RenderCoordinator.prototype.onFocus = function () {
        this._renderer.onFocus();
    };
    RenderCoordinator.prototype.onSelectionChanged = function (start, end, columnSelectMode) {
        this._renderer.onSelectionChanged(start, end, columnSelectMode);
    };
    RenderCoordinator.prototype.onCursorMove = function () {
        this._renderer.onCursorMove();
    };
    RenderCoordinator.prototype.onOptionsChanged = function () {
        this._renderer.onOptionsChanged();
    };
    RenderCoordinator.prototype.clear = function () {
        this._renderer.clear();
    };
    RenderCoordinator.prototype.registerCharacterJoiner = function (handler) {
        return this._renderer.registerCharacterJoiner(handler);
    };
    RenderCoordinator.prototype.deregisterCharacterJoiner = function (joinerId) {
        return this._renderer.deregisterCharacterJoiner(joinerId);
    };
    return RenderCoordinator;
}(Lifecycle_1.Disposable));
exports.RenderCoordinator = RenderCoordinator;
//# sourceMappingURL=RenderCoordinator.js.map