"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var Types_1 = require("../atlas/Types");
var BufferLine_1 = require("../../core/buffer/BufferLine");
exports.BOLD_CLASS = 'xterm-bold';
exports.DIM_CLASS = 'xterm-dim';
exports.ITALIC_CLASS = 'xterm-italic';
exports.UNDERLINE_CLASS = 'xterm-underline';
exports.CURSOR_CLASS = 'xterm-cursor';
exports.CURSOR_BLINK_CLASS = 'xterm-cursor-blink';
exports.CURSOR_STYLE_BLOCK_CLASS = 'xterm-cursor-block';
exports.CURSOR_STYLE_BAR_CLASS = 'xterm-cursor-bar';
exports.CURSOR_STYLE_UNDERLINE_CLASS = 'xterm-cursor-underline';
var DomRendererRowFactory = (function () {
    function DomRendererRowFactory(_terminalOptions, _document) {
        this._terminalOptions = _terminalOptions;
        this._document = _document;
        this._workCell = new BufferLine_1.CellData();
    }
    DomRendererRowFactory.prototype.createRow = function (lineData, isCursorRow, cursorStyle, cursorX, cursorBlink, cellWidth, cols) {
        var fragment = this._document.createDocumentFragment();
        var lineLength = 0;
        for (var x = Math.min(lineData.length, cols) - 1; x >= 0; x--) {
            if (lineData.loadCell(x, this._workCell).getCode() !== BufferLine_1.NULL_CELL_CODE || (isCursorRow && x === cursorX)) {
                lineLength = x + 1;
                break;
            }
        }
        for (var x = 0; x < lineLength; x++) {
            lineData.loadCell(x, this._workCell);
            var width = this._workCell.getWidth();
            if (width === 0) {
                continue;
            }
            var charElement = this._document.createElement('span');
            if (width > 1) {
                charElement.style.width = cellWidth * width + "px";
            }
            if (isCursorRow && x === cursorX) {
                charElement.classList.add(exports.CURSOR_CLASS);
                if (cursorBlink) {
                    charElement.classList.add(exports.CURSOR_BLINK_CLASS);
                }
                switch (cursorStyle) {
                    case 'bar':
                        charElement.classList.add(exports.CURSOR_STYLE_BAR_CLASS);
                        break;
                    case 'underline':
                        charElement.classList.add(exports.CURSOR_STYLE_UNDERLINE_CLASS);
                        break;
                    default:
                        charElement.classList.add(exports.CURSOR_STYLE_BLOCK_CLASS);
                        break;
                }
            }
            if (this._workCell.isBold() && this._terminalOptions.enableBold) {
                charElement.classList.add(exports.BOLD_CLASS);
            }
            if (this._workCell.isItalic()) {
                charElement.classList.add(exports.ITALIC_CLASS);
            }
            if (this._workCell.isDim()) {
                charElement.classList.add(exports.DIM_CLASS);
            }
            if (this._workCell.isUnderline()) {
                charElement.classList.add(exports.UNDERLINE_CLASS);
            }
            charElement.textContent = this._workCell.getChars() || BufferLine_1.WHITESPACE_CELL_CHAR;
            var swapColor = this._workCell.isInverse();
            if (this._workCell.isFgRGB()) {
                var style = charElement.getAttribute('style') || '';
                style += (swapColor ? 'background-' : '') + "color:rgb(" + (BufferLine_1.AttributeData.toColorRGB(this._workCell.getFgColor())).join(',') + ");";
                charElement.setAttribute('style', style);
            }
            else if (this._workCell.isFgPalette()) {
                var fg = this._workCell.getFgColor();
                if (this._workCell.isBold() && fg < 8 && !swapColor &&
                    this._terminalOptions.enableBold && this._terminalOptions.drawBoldTextInBrightColors) {
                    fg += 8;
                }
                charElement.classList.add("xterm-" + (swapColor ? 'b' : 'f') + "g-" + fg);
            }
            else if (swapColor) {
                charElement.classList.add("xterm-bg-" + Types_1.INVERTED_DEFAULT_COLOR);
            }
            if (this._workCell.isBgRGB()) {
                var style = charElement.getAttribute('style') || '';
                style += (swapColor ? '' : 'background-') + "color:rgb(" + (BufferLine_1.AttributeData.toColorRGB(this._workCell.getBgColor())).join(',') + ");";
                charElement.setAttribute('style', style);
            }
            else if (this._workCell.isBgPalette()) {
                charElement.classList.add("xterm-" + (swapColor ? 'f' : 'b') + "g-" + this._workCell.getBgColor());
            }
            else if (swapColor) {
                charElement.classList.add("xterm-fg-" + Types_1.INVERTED_DEFAULT_COLOR);
            }
            fragment.appendChild(charElement);
        }
        return fragment;
    };
    return DomRendererRowFactory;
}());
exports.DomRendererRowFactory = DomRendererRowFactory;
//# sourceMappingURL=DomRendererRowFactory.js.map