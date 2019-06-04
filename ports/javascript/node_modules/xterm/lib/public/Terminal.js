"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var Terminal_1 = require("../Terminal");
var Strings = require("../Strings");
var AddonManager_1 = require("./AddonManager");
var Terminal = (function () {
    function Terminal(options) {
        this._core = new Terminal_1.Terminal(options);
        this._addonManager = new AddonManager_1.AddonManager();
    }
    Object.defineProperty(Terminal.prototype, "onCursorMove", {
        get: function () { return this._core.onCursorMove; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onLineFeed", {
        get: function () { return this._core.onLineFeed; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onSelectionChange", {
        get: function () { return this._core.onSelectionChange; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onData", {
        get: function () { return this._core.onData; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onTitleChange", {
        get: function () { return this._core.onTitleChange; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onScroll", {
        get: function () { return this._core.onScroll; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onKey", {
        get: function () { return this._core.onKey; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onRender", {
        get: function () { return this._core.onRender; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onResize", {
        get: function () { return this._core.onResize; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "element", {
        get: function () { return this._core.element; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "textarea", {
        get: function () { return this._core.textarea; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "rows", {
        get: function () { return this._core.rows; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "cols", {
        get: function () { return this._core.cols; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "buffer", {
        get: function () { return new BufferApiView(this._core.buffer); },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "markers", {
        get: function () { return this._core.markers; },
        enumerable: true,
        configurable: true
    });
    Terminal.prototype.blur = function () {
        this._core.blur();
    };
    Terminal.prototype.focus = function () {
        this._core.focus();
    };
    Terminal.prototype.on = function (type, listener) {
        this._core.on(type, listener);
    };
    Terminal.prototype.off = function (type, listener) {
        this._core.off(type, listener);
    };
    Terminal.prototype.emit = function (type, data) {
        this._core.emit(type, data);
    };
    Terminal.prototype.addDisposableListener = function (type, handler) {
        return this._core.addDisposableListener(type, handler);
    };
    Terminal.prototype.resize = function (columns, rows) {
        this._core.resize(columns, rows);
    };
    Terminal.prototype.writeln = function (data) {
        this._core.writeln(data);
    };
    Terminal.prototype.open = function (parent) {
        this._core.open(parent);
    };
    Terminal.prototype.attachCustomKeyEventHandler = function (customKeyEventHandler) {
        this._core.attachCustomKeyEventHandler(customKeyEventHandler);
    };
    Terminal.prototype.addCsiHandler = function (flag, callback) {
        return this._core.addCsiHandler(flag, callback);
    };
    Terminal.prototype.addOscHandler = function (ident, callback) {
        return this._core.addOscHandler(ident, callback);
    };
    Terminal.prototype.registerLinkMatcher = function (regex, handler, options) {
        return this._core.registerLinkMatcher(regex, handler, options);
    };
    Terminal.prototype.deregisterLinkMatcher = function (matcherId) {
        this._core.deregisterLinkMatcher(matcherId);
    };
    Terminal.prototype.registerCharacterJoiner = function (handler) {
        return this._core.registerCharacterJoiner(handler);
    };
    Terminal.prototype.deregisterCharacterJoiner = function (joinerId) {
        this._core.deregisterCharacterJoiner(joinerId);
    };
    Terminal.prototype.addMarker = function (cursorYOffset) {
        return this._core.addMarker(cursorYOffset);
    };
    Terminal.prototype.hasSelection = function () {
        return this._core.hasSelection();
    };
    Terminal.prototype.select = function (column, row, length) {
        this._core.select(column, row, length);
    };
    Terminal.prototype.getSelection = function () {
        return this._core.getSelection();
    };
    Terminal.prototype.getSelectionPosition = function () {
        return this._core.getSelectionPosition();
    };
    Terminal.prototype.clearSelection = function () {
        this._core.clearSelection();
    };
    Terminal.prototype.selectAll = function () {
        this._core.selectAll();
    };
    Terminal.prototype.selectLines = function (start, end) {
        this._core.selectLines(start, end);
    };
    Terminal.prototype.dispose = function () {
        this._addonManager.dispose();
        this._core.dispose();
    };
    Terminal.prototype.destroy = function () {
        this._core.destroy();
    };
    Terminal.prototype.scrollLines = function (amount) {
        this._core.scrollLines(amount);
    };
    Terminal.prototype.scrollPages = function (pageCount) {
        this._core.scrollPages(pageCount);
    };
    Terminal.prototype.scrollToTop = function () {
        this._core.scrollToTop();
    };
    Terminal.prototype.scrollToBottom = function () {
        this._core.scrollToBottom();
    };
    Terminal.prototype.scrollToLine = function (line) {
        this._core.scrollToLine(line);
    };
    Terminal.prototype.clear = function () {
        this._core.clear();
    };
    Terminal.prototype.write = function (data) {
        this._core.write(data);
    };
    Terminal.prototype.writeUtf8 = function (data) {
        this._core.writeUtf8(data);
    };
    Terminal.prototype.getOption = function (key) {
        return this._core.getOption(key);
    };
    Terminal.prototype.setOption = function (key, value) {
        this._core.setOption(key, value);
    };
    Terminal.prototype.refresh = function (start, end) {
        this._core.refresh(start, end);
    };
    Terminal.prototype.reset = function () {
        this._core.reset();
    };
    Terminal.applyAddon = function (addon) {
        addon.apply(Terminal);
    };
    Terminal.prototype.loadAddon = function (addon) {
        return this._addonManager.loadAddon(this, addon);
    };
    Object.defineProperty(Terminal, "strings", {
        get: function () {
            return Strings;
        },
        enumerable: true,
        configurable: true
    });
    return Terminal;
}());
exports.Terminal = Terminal;
var BufferApiView = (function () {
    function BufferApiView(_buffer) {
        this._buffer = _buffer;
    }
    Object.defineProperty(BufferApiView.prototype, "cursorY", {
        get: function () { return this._buffer.y; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(BufferApiView.prototype, "cursorX", {
        get: function () { return this._buffer.x; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(BufferApiView.prototype, "viewportY", {
        get: function () { return this._buffer.ydisp; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(BufferApiView.prototype, "baseY", {
        get: function () { return this._buffer.ybase; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(BufferApiView.prototype, "length", {
        get: function () { return this._buffer.lines.length; },
        enumerable: true,
        configurable: true
    });
    BufferApiView.prototype.getLine = function (y) {
        var line = this._buffer.lines.get(y);
        if (!line) {
            return undefined;
        }
        return new BufferLineApiView(line);
    };
    return BufferApiView;
}());
var BufferLineApiView = (function () {
    function BufferLineApiView(_line) {
        this._line = _line;
    }
    Object.defineProperty(BufferLineApiView.prototype, "isWrapped", {
        get: function () { return this._line.isWrapped; },
        enumerable: true,
        configurable: true
    });
    BufferLineApiView.prototype.getCell = function (x) {
        if (x < 0 || x >= this._line.length) {
            return undefined;
        }
        return new BufferCellApiView(this._line, x);
    };
    BufferLineApiView.prototype.translateToString = function (trimRight, startColumn, endColumn) {
        return this._line.translateToString(trimRight, startColumn, endColumn);
    };
    return BufferLineApiView;
}());
var BufferCellApiView = (function () {
    function BufferCellApiView(_line, _x) {
        this._line = _line;
        this._x = _x;
    }
    Object.defineProperty(BufferCellApiView.prototype, "char", {
        get: function () { return this._line.getString(this._x); },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(BufferCellApiView.prototype, "width", {
        get: function () { return this._line.getWidth(this._x); },
        enumerable: true,
        configurable: true
    });
    return BufferCellApiView;
}());
//# sourceMappingURL=Terminal.js.map