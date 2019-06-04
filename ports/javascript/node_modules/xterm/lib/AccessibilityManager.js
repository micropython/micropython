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
var Strings = require("./Strings");
var Platform_1 = require("./common/Platform");
var RenderDebouncer_1 = require("./ui/RenderDebouncer");
var Lifecycle_1 = require("./ui/Lifecycle");
var Lifecycle_2 = require("./common/Lifecycle");
var ScreenDprMonitor_1 = require("./ui/ScreenDprMonitor");
var MAX_ROWS_TO_READ = 20;
var AccessibilityManager = (function (_super) {
    __extends(AccessibilityManager, _super);
    function AccessibilityManager(_terminal, _dimensions) {
        var _this = _super.call(this) || this;
        _this._terminal = _terminal;
        _this._dimensions = _dimensions;
        _this._liveRegionLineCount = 0;
        _this._charsToConsume = [];
        _this._charsToAnnounce = '';
        _this._accessibilityTreeRoot = document.createElement('div');
        _this._accessibilityTreeRoot.classList.add('xterm-accessibility');
        _this._rowContainer = document.createElement('div');
        _this._rowContainer.classList.add('xterm-accessibility-tree');
        _this._rowElements = [];
        for (var i = 0; i < _this._terminal.rows; i++) {
            _this._rowElements[i] = _this._createAccessibilityTreeNode();
            _this._rowContainer.appendChild(_this._rowElements[i]);
        }
        _this._topBoundaryFocusListener = function (e) { return _this._onBoundaryFocus(e, 0); };
        _this._bottomBoundaryFocusListener = function (e) { return _this._onBoundaryFocus(e, 1); };
        _this._rowElements[0].addEventListener('focus', _this._topBoundaryFocusListener);
        _this._rowElements[_this._rowElements.length - 1].addEventListener('focus', _this._bottomBoundaryFocusListener);
        _this._refreshRowsDimensions();
        _this._accessibilityTreeRoot.appendChild(_this._rowContainer);
        _this._renderRowsDebouncer = new RenderDebouncer_1.RenderDebouncer(_this._renderRows.bind(_this));
        _this._refreshRows();
        _this._liveRegion = document.createElement('div');
        _this._liveRegion.classList.add('live-region');
        _this._liveRegion.setAttribute('aria-live', 'assertive');
        _this._accessibilityTreeRoot.appendChild(_this._liveRegion);
        _this._terminal.element.insertAdjacentElement('afterbegin', _this._accessibilityTreeRoot);
        _this.register(_this._renderRowsDebouncer);
        _this.register(_this._terminal.onResize(function (e) { return _this._onResize(e.rows); }));
        _this.register(_this._terminal.onRender(function (e) { return _this._refreshRows(e.start, e.end); }));
        _this.register(_this._terminal.onScroll(function () { return _this._refreshRows(); }));
        _this.register(_this._terminal.addDisposableListener('a11y.char', function (char) { return _this._onChar(char); }));
        _this.register(_this._terminal.onLineFeed(function () { return _this._onChar('\n'); }));
        _this.register(_this._terminal.addDisposableListener('a11y.tab', function (spaceCount) { return _this._onTab(spaceCount); }));
        _this.register(_this._terminal.onKey(function (e) { return _this._onKey(e.key); }));
        _this.register(_this._terminal.addDisposableListener('blur', function () { return _this._clearLiveRegion(); }));
        _this._screenDprMonitor = new ScreenDprMonitor_1.ScreenDprMonitor();
        _this.register(_this._screenDprMonitor);
        _this._screenDprMonitor.setListener(function () { return _this._refreshRowsDimensions(); });
        _this.register(Lifecycle_1.addDisposableDomListener(window, 'resize', function () { return _this._refreshRowsDimensions(); }));
        return _this;
    }
    AccessibilityManager.prototype.dispose = function () {
        _super.prototype.dispose.call(this);
        this._terminal.element.removeChild(this._accessibilityTreeRoot);
        this._rowElements.length = 0;
    };
    AccessibilityManager.prototype._onBoundaryFocus = function (e, position) {
        var boundaryElement = e.target;
        var beforeBoundaryElement = this._rowElements[position === 0 ? 1 : this._rowElements.length - 2];
        var posInSet = boundaryElement.getAttribute('aria-posinset');
        var lastRowPos = position === 0 ? '1' : "" + this._terminal.buffer.lines.length;
        if (posInSet === lastRowPos) {
            return;
        }
        if (e.relatedTarget !== beforeBoundaryElement) {
            return;
        }
        var topBoundaryElement;
        var bottomBoundaryElement;
        if (position === 0) {
            topBoundaryElement = boundaryElement;
            bottomBoundaryElement = this._rowElements.pop();
            this._rowContainer.removeChild(bottomBoundaryElement);
        }
        else {
            topBoundaryElement = this._rowElements.shift();
            bottomBoundaryElement = boundaryElement;
            this._rowContainer.removeChild(topBoundaryElement);
        }
        topBoundaryElement.removeEventListener('focus', this._topBoundaryFocusListener);
        bottomBoundaryElement.removeEventListener('focus', this._bottomBoundaryFocusListener);
        if (position === 0) {
            var newElement = this._createAccessibilityTreeNode();
            this._rowElements.unshift(newElement);
            this._rowContainer.insertAdjacentElement('afterbegin', newElement);
        }
        else {
            var newElement = this._createAccessibilityTreeNode();
            this._rowElements.push(newElement);
            this._rowContainer.appendChild(newElement);
        }
        this._rowElements[0].addEventListener('focus', this._topBoundaryFocusListener);
        this._rowElements[this._rowElements.length - 1].addEventListener('focus', this._bottomBoundaryFocusListener);
        this._terminal.scrollLines(position === 0 ? -1 : 1);
        this._rowElements[position === 0 ? 1 : this._rowElements.length - 2].focus();
        e.preventDefault();
        e.stopImmediatePropagation();
    };
    AccessibilityManager.prototype._onResize = function (rows) {
        this._rowElements[this._rowElements.length - 1].removeEventListener('focus', this._bottomBoundaryFocusListener);
        for (var i = this._rowContainer.children.length; i < this._terminal.rows; i++) {
            this._rowElements[i] = this._createAccessibilityTreeNode();
            this._rowContainer.appendChild(this._rowElements[i]);
        }
        while (this._rowElements.length > rows) {
            this._rowContainer.removeChild(this._rowElements.pop());
        }
        this._rowElements[this._rowElements.length - 1].addEventListener('focus', this._bottomBoundaryFocusListener);
        this._refreshRowsDimensions();
    };
    AccessibilityManager.prototype._createAccessibilityTreeNode = function () {
        var element = document.createElement('div');
        element.setAttribute('role', 'listitem');
        element.tabIndex = -1;
        this._refreshRowDimensions(element);
        return element;
    };
    AccessibilityManager.prototype._onTab = function (spaceCount) {
        for (var i = 0; i < spaceCount; i++) {
            this._onChar(' ');
        }
    };
    AccessibilityManager.prototype._onChar = function (char) {
        var _this = this;
        if (this._liveRegionLineCount < MAX_ROWS_TO_READ + 1) {
            if (this._charsToConsume.length > 0) {
                var shiftedChar = this._charsToConsume.shift();
                if (shiftedChar !== char) {
                    this._charsToAnnounce += char;
                }
            }
            else {
                this._charsToAnnounce += char;
            }
            if (char === '\n') {
                this._liveRegionLineCount++;
                if (this._liveRegionLineCount === MAX_ROWS_TO_READ + 1) {
                    this._liveRegion.textContent += Strings.tooMuchOutput;
                }
            }
            if (Platform_1.isMac) {
                if (this._liveRegion.textContent && this._liveRegion.textContent.length > 0 && !this._liveRegion.parentNode) {
                    setTimeout(function () {
                        _this._accessibilityTreeRoot.appendChild(_this._liveRegion);
                    }, 0);
                }
            }
        }
    };
    AccessibilityManager.prototype._clearLiveRegion = function () {
        this._liveRegion.textContent = '';
        this._liveRegionLineCount = 0;
        if (Platform_1.isMac) {
            if (this._liveRegion.parentNode) {
                this._accessibilityTreeRoot.removeChild(this._liveRegion);
            }
        }
    };
    AccessibilityManager.prototype._onKey = function (keyChar) {
        this._clearLiveRegion();
        this._charsToConsume.push(keyChar);
    };
    AccessibilityManager.prototype._refreshRows = function (start, end) {
        this._renderRowsDebouncer.refresh(start, end, this._terminal.rows);
    };
    AccessibilityManager.prototype._renderRows = function (start, end) {
        var buffer = this._terminal.buffer;
        var setSize = buffer.lines.length.toString();
        for (var i = start; i <= end; i++) {
            var lineData = buffer.translateBufferLineToString(buffer.ydisp + i, true);
            var posInSet = (buffer.ydisp + i + 1).toString();
            var element = this._rowElements[i];
            if (element) {
                element.textContent = lineData.length === 0 ? Strings.blankLine : lineData;
                element.setAttribute('aria-posinset', posInSet);
                element.setAttribute('aria-setsize', setSize);
            }
        }
        this._announceCharacters();
    };
    AccessibilityManager.prototype._refreshRowsDimensions = function () {
        if (!this._dimensions.actualCellHeight) {
            return;
        }
        if (this._rowElements.length !== this._terminal.rows) {
            this._onResize(this._terminal.rows);
        }
        for (var i = 0; i < this._terminal.rows; i++) {
            this._refreshRowDimensions(this._rowElements[i]);
        }
    };
    AccessibilityManager.prototype.setDimensions = function (dimensions) {
        this._dimensions = dimensions;
        this._refreshRowsDimensions();
    };
    AccessibilityManager.prototype._refreshRowDimensions = function (element) {
        element.style.height = this._dimensions.actualCellHeight + "px";
    };
    AccessibilityManager.prototype._announceCharacters = function () {
        if (this._charsToAnnounce.length === 0) {
            return;
        }
        this._liveRegion.textContent += this._charsToAnnounce;
        this._charsToAnnounce = '';
    };
    return AccessibilityManager;
}(Lifecycle_2.Disposable));
exports.AccessibilityManager = AccessibilityManager;
//# sourceMappingURL=AccessibilityManager.js.map