"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var Buffer_1 = require("./Buffer");
var EventEmitter2_1 = require("./common/EventEmitter2");
var BufferSet = (function () {
    function BufferSet(_terminal) {
        this._terminal = _terminal;
        this._onBufferActivate = new EventEmitter2_1.EventEmitter2();
        this._normal = new Buffer_1.Buffer(this._terminal, true);
        this._normal.fillViewportRows();
        this._alt = new Buffer_1.Buffer(this._terminal, false);
        this._activeBuffer = this._normal;
        this.setupTabStops();
    }
    Object.defineProperty(BufferSet.prototype, "onBufferActivate", {
        get: function () { return this._onBufferActivate.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(BufferSet.prototype, "alt", {
        get: function () {
            return this._alt;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(BufferSet.prototype, "active", {
        get: function () {
            return this._activeBuffer;
        },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(BufferSet.prototype, "normal", {
        get: function () {
            return this._normal;
        },
        enumerable: true,
        configurable: true
    });
    BufferSet.prototype.activateNormalBuffer = function () {
        if (this._activeBuffer === this._normal) {
            return;
        }
        this._normal.x = this._alt.x;
        this._normal.y = this._alt.y;
        this._alt.clear();
        this._activeBuffer = this._normal;
        this._onBufferActivate.fire({
            activeBuffer: this._normal,
            inactiveBuffer: this._alt
        });
    };
    BufferSet.prototype.activateAltBuffer = function (fillAttr) {
        if (this._activeBuffer === this._alt) {
            return;
        }
        this._alt.fillViewportRows(fillAttr);
        this._alt.x = this._normal.x;
        this._alt.y = this._normal.y;
        this._activeBuffer = this._alt;
        this._onBufferActivate.fire({
            activeBuffer: this._alt,
            inactiveBuffer: this._normal
        });
    };
    BufferSet.prototype.resize = function (newCols, newRows) {
        this._normal.resize(newCols, newRows);
        this._alt.resize(newCols, newRows);
    };
    BufferSet.prototype.setupTabStops = function (i) {
        this._normal.setupTabStops(i);
        this._alt.setupTabStops(i);
    };
    return BufferSet;
}());
exports.BufferSet = BufferSet;
//# sourceMappingURL=BufferSet.js.map