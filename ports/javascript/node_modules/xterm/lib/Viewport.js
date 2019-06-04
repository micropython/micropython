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
var Lifecycle_1 = require("./common/Lifecycle");
var Lifecycle_2 = require("./ui/Lifecycle");
var FALLBACK_SCROLL_BAR_WIDTH = 15;
var Viewport = (function (_super) {
    __extends(Viewport, _super);
    function Viewport(_terminal, _viewportElement, _scrollArea, _charMeasure, _dimensions) {
        var _this = _super.call(this) || this;
        _this._terminal = _terminal;
        _this._viewportElement = _viewportElement;
        _this._scrollArea = _scrollArea;
        _this._charMeasure = _charMeasure;
        _this._dimensions = _dimensions;
        _this.scrollBarWidth = 0;
        _this._currentRowHeight = 0;
        _this._lastRecordedBufferLength = 0;
        _this._lastRecordedViewportHeight = 0;
        _this._lastRecordedBufferHeight = 0;
        _this._lastScrollTop = 0;
        _this._wheelPartialScroll = 0;
        _this._refreshAnimationFrame = null;
        _this._ignoreNextScrollEvent = false;
        _this.scrollBarWidth = (_this._viewportElement.offsetWidth - _this._scrollArea.offsetWidth) || FALLBACK_SCROLL_BAR_WIDTH;
        _this.register(Lifecycle_2.addDisposableDomListener(_this._viewportElement, 'scroll', _this._onScroll.bind(_this)));
        setTimeout(function () { return _this.syncScrollArea(); }, 0);
        return _this;
    }
    Viewport.prototype.onDimensionsChance = function (dimensions) {
        this._dimensions = dimensions;
    };
    Viewport.prototype.onThemeChange = function (colors) {
        this._viewportElement.style.backgroundColor = colors.background.css;
    };
    Viewport.prototype._refresh = function () {
        var _this = this;
        if (this._refreshAnimationFrame === null) {
            this._refreshAnimationFrame = requestAnimationFrame(function () { return _this._innerRefresh(); });
        }
    };
    Viewport.prototype._innerRefresh = function () {
        if (this._charMeasure.height > 0) {
            this._currentRowHeight = this._dimensions.scaledCellHeight / window.devicePixelRatio;
            this._lastRecordedViewportHeight = this._viewportElement.offsetHeight;
            var newBufferHeight = Math.round(this._currentRowHeight * this._lastRecordedBufferLength) + (this._lastRecordedViewportHeight - this._dimensions.canvasHeight);
            if (this._lastRecordedBufferHeight !== newBufferHeight) {
                this._lastRecordedBufferHeight = newBufferHeight;
                this._scrollArea.style.height = this._lastRecordedBufferHeight + 'px';
            }
        }
        var scrollTop = this._terminal.buffer.ydisp * this._currentRowHeight;
        if (this._viewportElement.scrollTop !== scrollTop) {
            this._ignoreNextScrollEvent = true;
            this._viewportElement.scrollTop = scrollTop;
        }
        this._refreshAnimationFrame = null;
    };
    Viewport.prototype.syncScrollArea = function () {
        if (this._lastRecordedBufferLength !== this._terminal.buffer.lines.length) {
            this._lastRecordedBufferLength = this._terminal.buffer.lines.length;
            this._refresh();
            return;
        }
        if (this._lastRecordedViewportHeight !== this._dimensions.canvasHeight) {
            this._refresh();
            return;
        }
        var newScrollTop = this._terminal.buffer.ydisp * this._currentRowHeight;
        if (this._lastScrollTop !== newScrollTop) {
            this._refresh();
            return;
        }
        if (this._lastScrollTop !== this._viewportElement.scrollTop) {
            this._refresh();
            return;
        }
        if (this._dimensions.scaledCellHeight / window.devicePixelRatio !== this._currentRowHeight) {
            this._refresh();
            return;
        }
    };
    Viewport.prototype._onScroll = function (ev) {
        this._lastScrollTop = this._viewportElement.scrollTop;
        if (!this._viewportElement.offsetParent) {
            return;
        }
        if (this._ignoreNextScrollEvent) {
            this._ignoreNextScrollEvent = false;
            return;
        }
        var newRow = Math.round(this._lastScrollTop / this._currentRowHeight);
        var diff = newRow - this._terminal.buffer.ydisp;
        this._terminal.scrollLines(diff, true);
    };
    Viewport.prototype.onWheel = function (ev) {
        var amount = this._getPixelsScrolled(ev);
        if (amount === 0) {
            return;
        }
        this._viewportElement.scrollTop += amount;
        ev.preventDefault();
    };
    Viewport.prototype._getPixelsScrolled = function (ev) {
        if (ev.deltaY === 0) {
            return 0;
        }
        var amount = ev.deltaY;
        if (ev.deltaMode === WheelEvent.DOM_DELTA_LINE) {
            amount *= this._currentRowHeight;
        }
        else if (ev.deltaMode === WheelEvent.DOM_DELTA_PAGE) {
            amount *= this._currentRowHeight * this._terminal.rows;
        }
        return amount;
    };
    Viewport.prototype.getLinesScrolled = function (ev) {
        if (ev.deltaY === 0) {
            return 0;
        }
        var amount = ev.deltaY;
        if (ev.deltaMode === WheelEvent.DOM_DELTA_PIXEL) {
            amount /= this._currentRowHeight + 0.0;
            this._wheelPartialScroll += amount;
            amount = Math.floor(Math.abs(this._wheelPartialScroll)) * (this._wheelPartialScroll > 0 ? 1 : -1);
            this._wheelPartialScroll %= 1;
        }
        else if (ev.deltaMode === WheelEvent.DOM_DELTA_PAGE) {
            amount *= this._terminal.rows;
        }
        return amount;
    };
    Viewport.prototype.onTouchStart = function (ev) {
        this._lastTouchY = ev.touches[0].pageY;
    };
    Viewport.prototype.onTouchMove = function (ev) {
        var deltaY = this._lastTouchY - ev.touches[0].pageY;
        this._lastTouchY = ev.touches[0].pageY;
        if (deltaY === 0) {
            return;
        }
        this._viewportElement.scrollTop += deltaY;
        ev.preventDefault();
    };
    return Viewport;
}(Lifecycle_1.Disposable));
exports.Viewport = Viewport;
//# sourceMappingURL=Viewport.js.map