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
var Lifecycle_1 = require("./Lifecycle");
var EventEmitter = (function (_super) {
    __extends(EventEmitter, _super);
    function EventEmitter() {
        var _this = _super.call(this) || this;
        _this._events = _this._events || {};
        return _this;
    }
    EventEmitter.prototype.on = function (type, listener) {
        this._events[type] = this._events[type] || [];
        this._events[type].push(listener);
    };
    EventEmitter.prototype.addDisposableListener = function (type, handler) {
        var _this = this;
        this.on(type, handler);
        var disposed = false;
        return {
            dispose: function () {
                if (disposed) {
                    return;
                }
                _this.off(type, handler);
                disposed = true;
            }
        };
    };
    EventEmitter.prototype.off = function (type, listener) {
        if (!this._events[type]) {
            return;
        }
        var obj = this._events[type];
        var i = obj.length;
        while (i--) {
            if (obj[i] === listener) {
                obj.splice(i, 1);
                return;
            }
        }
    };
    EventEmitter.prototype.removeAllListeners = function (type) {
        if (this._events[type]) {
            delete this._events[type];
        }
    };
    EventEmitter.prototype.emit = function (type) {
        var args = [];
        for (var _i = 1; _i < arguments.length; _i++) {
            args[_i - 1] = arguments[_i];
        }
        if (!this._events[type]) {
            return;
        }
        var obj = this._events[type];
        for (var i = 0; i < obj.length; i++) {
            obj[i].apply(this, args);
        }
    };
    EventEmitter.prototype.emitMayRemoveListeners = function (type) {
        var args = [];
        for (var _i = 1; _i < arguments.length; _i++) {
            args[_i - 1] = arguments[_i];
        }
        if (!this._events[type]) {
            return;
        }
        var obj = this._events[type];
        var length = obj.length;
        for (var i = 0; i < obj.length; i++) {
            obj[i].apply(this, args);
            i -= length - obj.length;
            length = obj.length;
        }
    };
    EventEmitter.prototype.listeners = function (type) {
        return this._events[type] || [];
    };
    EventEmitter.prototype.dispose = function () {
        _super.prototype.dispose.call(this);
        this._events = {};
    };
    return EventEmitter;
}(Lifecycle_1.Disposable));
exports.EventEmitter = EventEmitter;
//# sourceMappingURL=EventEmitter.js.map