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
var Lifecycle_1 = require("../common/Lifecycle");
var ScreenDprMonitor = (function (_super) {
    __extends(ScreenDprMonitor, _super);
    function ScreenDprMonitor() {
        var _this = _super !== null && _super.apply(this, arguments) || this;
        _this._currentDevicePixelRatio = window.devicePixelRatio;
        return _this;
    }
    ScreenDprMonitor.prototype.setListener = function (listener) {
        var _this = this;
        if (this._listener) {
            this.clearListener();
        }
        this._listener = listener;
        this._outerListener = function () {
            if (!_this._listener) {
                return;
            }
            _this._listener(window.devicePixelRatio, _this._currentDevicePixelRatio);
            _this._updateDpr();
        };
        this._updateDpr();
    };
    ScreenDprMonitor.prototype.dispose = function () {
        _super.prototype.dispose.call(this);
        this.clearListener();
    };
    ScreenDprMonitor.prototype._updateDpr = function () {
        if (!this._resolutionMediaMatchList || !this._outerListener) {
            return;
        }
        this._resolutionMediaMatchList.removeListener(this._outerListener);
        this._currentDevicePixelRatio = window.devicePixelRatio;
        this._resolutionMediaMatchList = window.matchMedia("screen and (resolution: " + window.devicePixelRatio + "dppx)");
        this._resolutionMediaMatchList.addListener(this._outerListener);
    };
    ScreenDprMonitor.prototype.clearListener = function () {
        if (!this._resolutionMediaMatchList || !this._listener || !this._outerListener) {
            return;
        }
        this._resolutionMediaMatchList.removeListener(this._outerListener);
        this._resolutionMediaMatchList = undefined;
        this._listener = undefined;
        this._outerListener = undefined;
    };
    return ScreenDprMonitor;
}(Lifecycle_1.Disposable));
exports.ScreenDprMonitor = ScreenDprMonitor;
//# sourceMappingURL=ScreenDprMonitor.js.map