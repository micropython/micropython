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
var HOVER_DURATION = 500;
var MouseZoneManager = (function (_super) {
    __extends(MouseZoneManager, _super);
    function MouseZoneManager(_terminal) {
        var _this = _super.call(this) || this;
        _this._terminal = _terminal;
        _this._zones = [];
        _this._areZonesActive = false;
        _this._tooltipTimeout = null;
        _this._currentZone = null;
        _this._lastHoverCoords = [null, null];
        _this.register(Lifecycle_2.addDisposableDomListener(_this._terminal.element, 'mousedown', function (e) { return _this._onMouseDown(e); }));
        _this._mouseMoveListener = function (e) { return _this._onMouseMove(e); };
        _this._mouseLeaveListener = function (e) { return _this._onMouseLeave(e); };
        _this._clickListener = function (e) { return _this._onClick(e); };
        return _this;
    }
    MouseZoneManager.prototype.dispose = function () {
        _super.prototype.dispose.call(this);
        this._deactivate();
    };
    MouseZoneManager.prototype.add = function (zone) {
        this._zones.push(zone);
        if (this._zones.length === 1) {
            this._activate();
        }
    };
    MouseZoneManager.prototype.clearAll = function (start, end) {
        if (this._zones.length === 0) {
            return;
        }
        if (!end) {
            start = 0;
            end = this._terminal.rows - 1;
        }
        for (var i = 0; i < this._zones.length; i++) {
            var zone = this._zones[i];
            if ((zone.y1 > start && zone.y1 <= end + 1) ||
                (zone.y2 > start && zone.y2 <= end + 1) ||
                (zone.y1 < start && zone.y2 > end + 1)) {
                if (this._currentZone && this._currentZone === zone) {
                    this._currentZone.leaveCallback();
                    this._currentZone = null;
                }
                this._zones.splice(i--, 1);
            }
        }
        if (this._zones.length === 0) {
            this._deactivate();
        }
    };
    MouseZoneManager.prototype._activate = function () {
        if (!this._areZonesActive) {
            this._areZonesActive = true;
            this._terminal.element.addEventListener('mousemove', this._mouseMoveListener);
            this._terminal.element.addEventListener('mouseleave', this._mouseLeaveListener);
            this._terminal.element.addEventListener('click', this._clickListener);
        }
    };
    MouseZoneManager.prototype._deactivate = function () {
        if (this._areZonesActive) {
            this._areZonesActive = false;
            this._terminal.element.removeEventListener('mousemove', this._mouseMoveListener);
            this._terminal.element.removeEventListener('mouseleave', this._mouseLeaveListener);
            this._terminal.element.removeEventListener('click', this._clickListener);
        }
    };
    MouseZoneManager.prototype._onMouseMove = function (e) {
        if (this._lastHoverCoords[0] !== e.pageX || this._lastHoverCoords[1] !== e.pageY) {
            this._onHover(e);
            this._lastHoverCoords = [e.pageX, e.pageY];
        }
    };
    MouseZoneManager.prototype._onHover = function (e) {
        var _this = this;
        var zone = this._findZoneEventAt(e);
        if (zone === this._currentZone) {
            return;
        }
        if (this._currentZone) {
            this._currentZone.leaveCallback();
            this._currentZone = null;
            if (this._tooltipTimeout) {
                clearTimeout(this._tooltipTimeout);
            }
        }
        if (!zone) {
            return;
        }
        this._currentZone = zone;
        if (zone.hoverCallback) {
            zone.hoverCallback(e);
        }
        this._tooltipTimeout = setTimeout(function () { return _this._onTooltip(e); }, HOVER_DURATION);
    };
    MouseZoneManager.prototype._onTooltip = function (e) {
        this._tooltipTimeout = null;
        var zone = this._findZoneEventAt(e);
        if (zone && zone.tooltipCallback) {
            zone.tooltipCallback(e);
        }
    };
    MouseZoneManager.prototype._onMouseDown = function (e) {
        this._initialSelectionLength = this._terminal.getSelection().length;
        if (!this._areZonesActive) {
            return;
        }
        var zone = this._findZoneEventAt(e);
        if (zone) {
            if (zone.willLinkActivate(e)) {
                e.preventDefault();
                e.stopImmediatePropagation();
            }
        }
    };
    MouseZoneManager.prototype._onMouseLeave = function (e) {
        if (this._currentZone) {
            this._currentZone.leaveCallback();
            this._currentZone = null;
            if (this._tooltipTimeout) {
                clearTimeout(this._tooltipTimeout);
            }
        }
    };
    MouseZoneManager.prototype._onClick = function (e) {
        var zone = this._findZoneEventAt(e);
        var currentSelectionLength = this._terminal.getSelection().length;
        if (zone && currentSelectionLength === this._initialSelectionLength) {
            zone.clickCallback(e);
            e.preventDefault();
            e.stopImmediatePropagation();
        }
    };
    MouseZoneManager.prototype._findZoneEventAt = function (e) {
        var coords = this._terminal.mouseHelper.getCoords(e, this._terminal.screenElement, this._terminal.charMeasure, this._terminal.cols, this._terminal.rows);
        if (!coords) {
            return null;
        }
        var x = coords[0];
        var y = coords[1];
        for (var i = 0; i < this._zones.length; i++) {
            var zone = this._zones[i];
            if (zone.y1 === zone.y2) {
                if (y === zone.y1 && x >= zone.x1 && x < zone.x2) {
                    return zone;
                }
            }
            else {
                if ((y === zone.y1 && x >= zone.x1) ||
                    (y === zone.y2 && x < zone.x2) ||
                    (y > zone.y1 && y < zone.y2)) {
                    return zone;
                }
            }
        }
        return null;
    };
    return MouseZoneManager;
}(Lifecycle_1.Disposable));
exports.MouseZoneManager = MouseZoneManager;
var MouseZone = (function () {
    function MouseZone(x1, y1, x2, y2, clickCallback, hoverCallback, tooltipCallback, leaveCallback, willLinkActivate) {
        this.x1 = x1;
        this.y1 = y1;
        this.x2 = x2;
        this.y2 = y2;
        this.clickCallback = clickCallback;
        this.hoverCallback = hoverCallback;
        this.tooltipCallback = tooltipCallback;
        this.leaveCallback = leaveCallback;
        this.willLinkActivate = willLinkActivate;
    }
    return MouseZone;
}());
exports.MouseZone = MouseZone;
//# sourceMappingURL=MouseZoneManager.js.map