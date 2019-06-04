"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var MouseZoneManager_1 = require("./MouseZoneManager");
var CharWidth_1 = require("./CharWidth");
var EventEmitter2_1 = require("./common/EventEmitter2");
var Linkifier = (function () {
    function Linkifier(_terminal) {
        this._terminal = _terminal;
        this._linkMatchers = [];
        this._nextLinkMatcherId = 0;
        this._onLinkHover = new EventEmitter2_1.EventEmitter2();
        this._onLinkLeave = new EventEmitter2_1.EventEmitter2();
        this._onLinkTooltip = new EventEmitter2_1.EventEmitter2();
        this._rowsToLinkify = {
            start: null,
            end: null
        };
    }
    Object.defineProperty(Linkifier.prototype, "onLinkHover", {
        get: function () { return this._onLinkHover.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Linkifier.prototype, "onLinkLeave", {
        get: function () { return this._onLinkLeave.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Linkifier.prototype, "onLinkTooltip", {
        get: function () { return this._onLinkTooltip.event; },
        enumerable: true,
        configurable: true
    });
    Linkifier.prototype.attachToDom = function (mouseZoneManager) {
        this._mouseZoneManager = mouseZoneManager;
    };
    Linkifier.prototype.linkifyRows = function (start, end) {
        var _this = this;
        if (!this._mouseZoneManager) {
            return;
        }
        if (this._rowsToLinkify.start === null) {
            this._rowsToLinkify.start = start;
            this._rowsToLinkify.end = end;
        }
        else {
            this._rowsToLinkify.start = Math.min(this._rowsToLinkify.start, start);
            this._rowsToLinkify.end = Math.max(this._rowsToLinkify.end, end);
        }
        this._mouseZoneManager.clearAll(start, end);
        if (this._rowsTimeoutId) {
            clearTimeout(this._rowsTimeoutId);
        }
        this._rowsTimeoutId = setTimeout(function () { return _this._linkifyRows(); }, Linkifier.TIME_BEFORE_LINKIFY);
    };
    Linkifier.prototype._linkifyRows = function () {
        this._rowsTimeoutId = null;
        var buffer = this._terminal.buffer;
        var absoluteRowIndexStart = buffer.ydisp + this._rowsToLinkify.start;
        if (absoluteRowIndexStart >= buffer.lines.length) {
            return;
        }
        var absoluteRowIndexEnd = buffer.ydisp + Math.min(this._rowsToLinkify.end, this._terminal.rows) + 1;
        var overscanLineLimit = Math.ceil(Linkifier.OVERSCAN_CHAR_LIMIT / this._terminal.cols);
        var iterator = this._terminal.buffer.iterator(false, absoluteRowIndexStart, absoluteRowIndexEnd, overscanLineLimit, overscanLineLimit);
        while (iterator.hasNext()) {
            var lineData = iterator.next();
            for (var i = 0; i < this._linkMatchers.length; i++) {
                this._doLinkifyRow(lineData.range.first, lineData.content, this._linkMatchers[i]);
            }
        }
        this._rowsToLinkify.start = null;
        this._rowsToLinkify.end = null;
    };
    Linkifier.prototype.registerLinkMatcher = function (regex, handler, options) {
        if (options === void 0) { options = {}; }
        if (!handler) {
            throw new Error('handler must be defined');
        }
        var matcher = {
            id: this._nextLinkMatcherId++,
            regex: regex,
            handler: handler,
            matchIndex: options.matchIndex,
            validationCallback: options.validationCallback,
            hoverTooltipCallback: options.tooltipCallback,
            hoverLeaveCallback: options.leaveCallback,
            willLinkActivate: options.willLinkActivate,
            priority: options.priority || 0
        };
        this._addLinkMatcherToList(matcher);
        return matcher.id;
    };
    Linkifier.prototype._addLinkMatcherToList = function (matcher) {
        if (this._linkMatchers.length === 0) {
            this._linkMatchers.push(matcher);
            return;
        }
        for (var i = this._linkMatchers.length - 1; i >= 0; i--) {
            if (matcher.priority <= this._linkMatchers[i].priority) {
                this._linkMatchers.splice(i + 1, 0, matcher);
                return;
            }
        }
        this._linkMatchers.splice(0, 0, matcher);
    };
    Linkifier.prototype.deregisterLinkMatcher = function (matcherId) {
        for (var i = 0; i < this._linkMatchers.length; i++) {
            if (this._linkMatchers[i].id === matcherId) {
                this._linkMatchers.splice(i, 1);
                return true;
            }
        }
        return false;
    };
    Linkifier.prototype._doLinkifyRow = function (rowIndex, text, matcher) {
        var _this = this;
        var rex = new RegExp(matcher.regex.source, matcher.regex.flags + 'g');
        var match;
        var stringIndex = -1;
        var _loop_1 = function () {
            var uri = match[typeof matcher.matchIndex !== 'number' ? 0 : matcher.matchIndex];
            if (!uri) {
                if (this_1._terminal.debug) {
                    console.log({ match: match, matcher: matcher });
                    throw new Error('match found without corresponding matchIndex');
                }
                return "break";
            }
            stringIndex = text.indexOf(uri, stringIndex + 1);
            rex.lastIndex = stringIndex + uri.length;
            if (stringIndex < 0) {
                return "break";
            }
            var bufferIndex = this_1._terminal.buffer.stringIndexToBufferIndex(rowIndex, stringIndex);
            if (bufferIndex[0] < 0) {
                return "break";
            }
            var line = this_1._terminal.buffer.lines.get(bufferIndex[0]);
            var attr = line.getFg(bufferIndex[1]);
            var fg;
            if (attr) {
                fg = (attr >> 9) & 0x1ff;
            }
            if (matcher.validationCallback) {
                matcher.validationCallback(uri, function (isValid) {
                    if (_this._rowsTimeoutId) {
                        return;
                    }
                    if (isValid) {
                        _this._addLink(bufferIndex[1], bufferIndex[0] - _this._terminal.buffer.ydisp, uri, matcher, fg);
                    }
                });
            }
            else {
                this_1._addLink(bufferIndex[1], bufferIndex[0] - this_1._terminal.buffer.ydisp, uri, matcher, fg);
            }
        };
        var this_1 = this;
        while ((match = rex.exec(text)) !== null) {
            var state_1 = _loop_1();
            if (state_1 === "break")
                break;
        }
    };
    Linkifier.prototype._addLink = function (x, y, uri, matcher, fg) {
        var _this = this;
        var width = CharWidth_1.getStringCellWidth(uri);
        var x1 = x % this._terminal.cols;
        var y1 = y + Math.floor(x / this._terminal.cols);
        var x2 = (x1 + width) % this._terminal.cols;
        var y2 = y1 + Math.floor((x1 + width) / this._terminal.cols);
        if (x2 === 0) {
            x2 = this._terminal.cols;
            y2--;
        }
        this._mouseZoneManager.add(new MouseZoneManager_1.MouseZone(x1 + 1, y1 + 1, x2 + 1, y2 + 1, function (e) {
            if (matcher.handler) {
                return matcher.handler(e, uri);
            }
            window.open(uri, '_blank');
        }, function () {
            _this._onLinkHover.fire(_this._createLinkHoverEvent(x1, y1, x2, y2, fg));
            _this._terminal.element.classList.add('xterm-cursor-pointer');
        }, function (e) {
            _this._onLinkTooltip.fire(_this._createLinkHoverEvent(x1, y1, x2, y2, fg));
            if (matcher.hoverTooltipCallback) {
                matcher.hoverTooltipCallback(e, uri);
            }
        }, function () {
            _this._onLinkLeave.fire(_this._createLinkHoverEvent(x1, y1, x2, y2, fg));
            _this._terminal.element.classList.remove('xterm-cursor-pointer');
            if (matcher.hoverLeaveCallback) {
                matcher.hoverLeaveCallback();
            }
        }, function (e) {
            if (matcher.willLinkActivate) {
                return matcher.willLinkActivate(e, uri);
            }
            return true;
        }));
    };
    Linkifier.prototype._createLinkHoverEvent = function (x1, y1, x2, y2, fg) {
        return { x1: x1, y1: y1, x2: x2, y2: y2, cols: this._terminal.cols, fg: fg };
    };
    Linkifier.TIME_BEFORE_LINKIFY = 200;
    Linkifier.OVERSCAN_CHAR_LIMIT = 2000;
    return Linkifier;
}());
exports.Linkifier = Linkifier;
//# sourceMappingURL=Linkifier.js.map