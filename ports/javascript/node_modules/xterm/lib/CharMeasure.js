"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var EventEmitter2_1 = require("./common/EventEmitter2");
var CharMeasure = (function () {
    function CharMeasure(document, parentElement) {
        this._onCharSizeChanged = new EventEmitter2_1.EventEmitter2();
        this._document = document;
        this._parentElement = parentElement;
        this._measureElement = this._document.createElement('span');
        this._measureElement.classList.add('xterm-char-measure-element');
        this._measureElement.textContent = 'W';
        this._measureElement.setAttribute('aria-hidden', 'true');
        this._parentElement.appendChild(this._measureElement);
    }
    Object.defineProperty(CharMeasure.prototype, "onCharSizeChanged", {
        get: function () { return this._onCharSizeChanged.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(CharMeasure.prototype, "width", {
        get: function () {
            return this._width;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(CharMeasure.prototype, "height", {
        get: function () {
            return this._height;
        },
        enumerable: true,
        configurable: true
    });
    CharMeasure.prototype.measure = function (options) {
        this._measureElement.style.fontFamily = options.fontFamily;
        this._measureElement.style.fontSize = options.fontSize + "px";
        var geometry = this._measureElement.getBoundingClientRect();
        if (geometry.width === 0 || geometry.height === 0) {
            return;
        }
        var adjustedHeight = Math.ceil(geometry.height);
        if (this._width !== geometry.width || this._height !== adjustedHeight) {
            this._width = geometry.width;
            this._height = adjustedHeight;
            this._onCharSizeChanged.fire();
        }
    };
    return CharMeasure;
}());
exports.CharMeasure = CharMeasure;
//# sourceMappingURL=CharMeasure.js.map