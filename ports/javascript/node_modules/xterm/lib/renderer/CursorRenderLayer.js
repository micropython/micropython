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
var BufferLine_1 = require("../core/buffer/BufferLine");
var BLINK_INTERVAL = 600;
var CursorRenderLayer = (function (_super) {
    __extends(CursorRenderLayer, _super);
    function CursorRenderLayer(container, zIndex, colors) {
        var _this = _super.call(this, container, 'cursor', zIndex, true, colors) || this;
        _this._cell = new BufferLine_1.CellData();
        _this._state = {
            x: null,
            y: null,
            isFocused: null,
            style: null,
            width: null
        };
        _this._cursorRenderers = {
            'bar': _this._renderBarCursor.bind(_this),
            'block': _this._renderBlockCursor.bind(_this),
            'underline': _this._renderUnderlineCursor.bind(_this)
        };
        return _this;
    }
    CursorRenderLayer.prototype.resize = function (terminal, dim) {
        _super.prototype.resize.call(this, terminal, dim);
        this._state = {
            x: null,
            y: null,
            isFocused: null,
            style: null,
            width: null
        };
    };
    CursorRenderLayer.prototype.reset = function (terminal) {
        this._clearCursor();
        if (this._cursorBlinkStateManager) {
            this._cursorBlinkStateManager.dispose();
            this._cursorBlinkStateManager = null;
            this.onOptionsChanged(terminal);
        }
    };
    CursorRenderLayer.prototype.onBlur = function (terminal) {
        if (this._cursorBlinkStateManager) {
            this._cursorBlinkStateManager.pause();
        }
        terminal.refresh(terminal.buffer.y, terminal.buffer.y);
    };
    CursorRenderLayer.prototype.onFocus = function (terminal) {
        if (this._cursorBlinkStateManager) {
            this._cursorBlinkStateManager.resume(terminal);
        }
        else {
            terminal.refresh(terminal.buffer.y, terminal.buffer.y);
        }
    };
    CursorRenderLayer.prototype.onOptionsChanged = function (terminal) {
        var _this = this;
        if (terminal.options.cursorBlink) {
            if (!this._cursorBlinkStateManager) {
                this._cursorBlinkStateManager = new CursorBlinkStateManager(terminal, function () {
                    _this._render(terminal, true);
                });
            }
        }
        else {
            if (this._cursorBlinkStateManager) {
                this._cursorBlinkStateManager.dispose();
                this._cursorBlinkStateManager = null;
            }
            terminal.refresh(terminal.buffer.y, terminal.buffer.y);
        }
    };
    CursorRenderLayer.prototype.onCursorMove = function (terminal) {
        if (this._cursorBlinkStateManager) {
            this._cursorBlinkStateManager.restartBlinkAnimation(terminal);
        }
    };
    CursorRenderLayer.prototype.onGridChanged = function (terminal, startRow, endRow) {
        if (!this._cursorBlinkStateManager || this._cursorBlinkStateManager.isPaused) {
            this._render(terminal, false);
        }
        else {
            this._cursorBlinkStateManager.restartBlinkAnimation(terminal);
        }
    };
    CursorRenderLayer.prototype._render = function (terminal, triggeredByAnimationFrame) {
        if (!terminal.cursorState || terminal.cursorHidden) {
            this._clearCursor();
            return;
        }
        var cursorY = terminal.buffer.ybase + terminal.buffer.y;
        var viewportRelativeCursorY = cursorY - terminal.buffer.ydisp;
        if (viewportRelativeCursorY < 0 || viewportRelativeCursorY >= terminal.rows) {
            this._clearCursor();
            return;
        }
        terminal.buffer.lines.get(cursorY).loadCell(terminal.buffer.x, this._cell);
        if (this._cell.content === undefined) {
            return;
        }
        if (!terminal.isFocused) {
            this._clearCursor();
            this._ctx.save();
            this._ctx.fillStyle = this._colors.cursor.css;
            this._renderBlurCursor(terminal, terminal.buffer.x, viewportRelativeCursorY, this._cell);
            this._ctx.restore();
            this._state.x = terminal.buffer.x;
            this._state.y = viewportRelativeCursorY;
            this._state.isFocused = false;
            this._state.style = terminal.options.cursorStyle;
            this._state.width = this._cell.getWidth();
            return;
        }
        if (this._cursorBlinkStateManager && !this._cursorBlinkStateManager.isCursorVisible) {
            this._clearCursor();
            return;
        }
        if (this._state) {
            if (this._state.x === terminal.buffer.x &&
                this._state.y === viewportRelativeCursorY &&
                this._state.isFocused === terminal.isFocused &&
                this._state.style === terminal.options.cursorStyle &&
                this._state.width === this._cell.getWidth()) {
                return;
            }
            this._clearCursor();
        }
        this._ctx.save();
        this._cursorRenderers[terminal.options.cursorStyle || 'block'](terminal, terminal.buffer.x, viewportRelativeCursorY, this._cell);
        this._ctx.restore();
        this._state.x = terminal.buffer.x;
        this._state.y = viewportRelativeCursorY;
        this._state.isFocused = false;
        this._state.style = terminal.options.cursorStyle;
        this._state.width = this._cell.getWidth();
    };
    CursorRenderLayer.prototype._clearCursor = function () {
        if (this._state) {
            this.clearCells(this._state.x, this._state.y, this._state.width, 1);
            this._state = {
                x: null,
                y: null,
                isFocused: null,
                style: null,
                width: null
            };
        }
    };
    CursorRenderLayer.prototype._renderBarCursor = function (terminal, x, y, cell) {
        this._ctx.save();
        this._ctx.fillStyle = this._colors.cursor.css;
        this.fillLeftLineAtCell(x, y);
        this._ctx.restore();
    };
    CursorRenderLayer.prototype._renderBlockCursor = function (terminal, x, y, cell) {
        this._ctx.save();
        this._ctx.fillStyle = this._colors.cursor.css;
        this.fillCells(x, y, cell.getWidth(), 1);
        this._ctx.fillStyle = this._colors.cursorAccent.css;
        this.fillCharTrueColor(terminal, cell, x, y);
        this._ctx.restore();
    };
    CursorRenderLayer.prototype._renderUnderlineCursor = function (terminal, x, y, cell) {
        this._ctx.save();
        this._ctx.fillStyle = this._colors.cursor.css;
        this.fillBottomLineAtCells(x, y);
        this._ctx.restore();
    };
    CursorRenderLayer.prototype._renderBlurCursor = function (terminal, x, y, cell) {
        this._ctx.save();
        this._ctx.strokeStyle = this._colors.cursor.css;
        this.strokeRectAtCell(x, y, cell.getWidth(), 1);
        this._ctx.restore();
    };
    return CursorRenderLayer;
}(BaseRenderLayer_1.BaseRenderLayer));
exports.CursorRenderLayer = CursorRenderLayer;
var CursorBlinkStateManager = (function () {
    function CursorBlinkStateManager(terminal, _renderCallback) {
        this._renderCallback = _renderCallback;
        this.isCursorVisible = true;
        if (terminal.isFocused) {
            this._restartInterval();
        }
    }
    Object.defineProperty(CursorBlinkStateManager.prototype, "isPaused", {
        get: function () { return !(this._blinkStartTimeout || this._blinkInterval); },
        enumerable: true,
        configurable: true
    });
    CursorBlinkStateManager.prototype.dispose = function () {
        if (this._blinkInterval) {
            window.clearInterval(this._blinkInterval);
            this._blinkInterval = null;
        }
        if (this._blinkStartTimeout) {
            window.clearTimeout(this._blinkStartTimeout);
            this._blinkStartTimeout = null;
        }
        if (this._animationFrame) {
            window.cancelAnimationFrame(this._animationFrame);
            this._animationFrame = null;
        }
    };
    CursorBlinkStateManager.prototype.restartBlinkAnimation = function (terminal) {
        var _this = this;
        if (this.isPaused) {
            return;
        }
        this._animationTimeRestarted = Date.now();
        this.isCursorVisible = true;
        if (!this._animationFrame) {
            this._animationFrame = window.requestAnimationFrame(function () {
                _this._renderCallback();
                _this._animationFrame = null;
            });
        }
    };
    CursorBlinkStateManager.prototype._restartInterval = function (timeToStart) {
        var _this = this;
        if (timeToStart === void 0) { timeToStart = BLINK_INTERVAL; }
        if (this._blinkInterval) {
            window.clearInterval(this._blinkInterval);
        }
        this._blinkStartTimeout = setTimeout(function () {
            if (_this._animationTimeRestarted) {
                var time = BLINK_INTERVAL - (Date.now() - _this._animationTimeRestarted);
                _this._animationTimeRestarted = null;
                if (time > 0) {
                    _this._restartInterval(time);
                    return;
                }
            }
            _this.isCursorVisible = false;
            _this._animationFrame = window.requestAnimationFrame(function () {
                _this._renderCallback();
                _this._animationFrame = null;
            });
            _this._blinkInterval = setInterval(function () {
                if (_this._animationTimeRestarted) {
                    var time = BLINK_INTERVAL - (Date.now() - _this._animationTimeRestarted);
                    _this._animationTimeRestarted = null;
                    _this._restartInterval(time);
                    return;
                }
                _this.isCursorVisible = !_this.isCursorVisible;
                _this._animationFrame = window.requestAnimationFrame(function () {
                    _this._renderCallback();
                    _this._animationFrame = null;
                });
            }, BLINK_INTERVAL);
        }, timeToStart);
    };
    CursorBlinkStateManager.prototype.pause = function () {
        this.isCursorVisible = true;
        if (this._blinkInterval) {
            window.clearInterval(this._blinkInterval);
            this._blinkInterval = null;
        }
        if (this._blinkStartTimeout) {
            window.clearTimeout(this._blinkStartTimeout);
            this._blinkStartTimeout = null;
        }
        if (this._animationFrame) {
            window.cancelAnimationFrame(this._animationFrame);
            this._animationFrame = null;
        }
    };
    CursorBlinkStateManager.prototype.resume = function (terminal) {
        this._animationTimeRestarted = null;
        this._restartInterval();
        this.restartBlinkAnimation(terminal);
    };
    return CursorBlinkStateManager;
}());
//# sourceMappingURL=CursorRenderLayer.js.map