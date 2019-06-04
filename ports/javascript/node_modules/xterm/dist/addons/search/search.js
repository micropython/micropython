(function(f){if(typeof exports==="object"&&typeof module!=="undefined"){module.exports=f()}else if(typeof define==="function"&&define.amd){define([],f)}else{var g;if(typeof window!=="undefined"){g=window}else if(typeof global!=="undefined"){g=global}else if(typeof self!=="undefined"){g=self}else{g=this}g.search = f()}})(function(){var define,module,exports;return (function(){function r(e,n,t){function o(i,f){if(!n[i]){if(!e[i]){var c="function"==typeof require&&require;if(!f&&c)return c(i,!0);if(u)return u(i,!0);var a=new Error("Cannot find module '"+i+"'");throw a.code="MODULE_NOT_FOUND",a}var p=n[i]={exports:{}};e[i][0].call(p.exports,function(r){var n=e[i][1][r];return o(n||r)},p,p.exports,r,e,n,t)}return n[i].exports}for(var u="function"==typeof require&&require,i=0;i<t.length;i++)o(t[i]);return o}return r})()({1:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var NON_WORD_CHARACTERS = ' ~!@#$%^&*()+`-=[]{}|\;:"\',./<>?';
var LINES_CACHE_TIME_TO_LIVE = 15 * 1000;
var SearchHelper = (function () {
    function SearchHelper(_terminal) {
        this._terminal = _terminal;
        this._linesCache = null;
        this._linesCacheTimeoutId = 0;
        this._destroyLinesCache = this._destroyLinesCache.bind(this);
    }
    SearchHelper.prototype.findNext = function (term, searchOptions) {
        var incremental = searchOptions.incremental;
        var result;
        if (!term || term.length === 0) {
            this._terminal.clearSelection();
            return false;
        }
        var startCol = 0;
        var startRow = this._terminal.buffer.viewportY;
        if (this._terminal.hasSelection()) {
            var currentSelection = this._terminal.getSelectionPosition();
            startRow = incremental ? currentSelection.startRow : currentSelection.endRow;
            startCol = incremental ? currentSelection.startColumn : currentSelection.endColumn;
        }
        this._initLinesCache();
        var findingRow = startRow;
        var cumulativeCols = startCol;
        while (this._terminal.buffer.getLine(findingRow).isWrapped) {
            findingRow--;
            cumulativeCols += this._terminal.cols;
        }
        result = this._findInLine(term, findingRow, cumulativeCols, searchOptions);
        if (!result) {
            for (var y = startRow + 1; y < this._terminal.buffer.baseY + this._terminal.rows; y++) {
                result = this._findInLine(term, y, 0, searchOptions);
                if (result) {
                    break;
                }
            }
        }
        if (!result) {
            for (var y = 0; y < findingRow; y++) {
                result = this._findInLine(term, y, 0, searchOptions);
                if (result) {
                    break;
                }
            }
        }
        return this._selectResult(result);
    };
    SearchHelper.prototype.findPrevious = function (term, searchOptions) {
        var result;
        if (!term || term.length === 0) {
            this._terminal.clearSelection();
            return false;
        }
        var isReverseSearch = true;
        var startRow = this._terminal.buffer.viewportY + this._terminal.rows - 1;
        var startCol = this._terminal.cols;
        if (this._terminal.hasSelection()) {
            var currentSelection = this._terminal.getSelectionPosition();
            startRow = currentSelection.startRow;
            startCol = currentSelection.startColumn;
        }
        this._initLinesCache();
        result = this._findInLine(term, startRow, startCol, searchOptions, isReverseSearch);
        if (!result) {
            var cumulativeCols = this._terminal.cols;
            if (this._terminal.buffer.getLine(startRow).isWrapped) {
                cumulativeCols += startCol;
            }
            for (var y = startRow - 1; y >= 0; y--) {
                result = this._findInLine(term, y, cumulativeCols, searchOptions, isReverseSearch);
                if (result) {
                    break;
                }
                if (this._terminal.buffer.getLine(y).isWrapped) {
                    cumulativeCols += this._terminal.cols;
                }
                else {
                    cumulativeCols = this._terminal.cols;
                }
            }
        }
        if (!result) {
            var searchFrom = this._terminal.buffer.baseY + this._terminal.rows - 1;
            var cumulativeCols = this._terminal.cols;
            for (var y = searchFrom; y >= startRow; y--) {
                result = this._findInLine(term, y, cumulativeCols, searchOptions, isReverseSearch);
                if (result) {
                    break;
                }
                if (this._terminal.buffer.getLine(y).isWrapped) {
                    cumulativeCols += this._terminal.cols;
                }
                else {
                    cumulativeCols = this._terminal.cols;
                }
            }
        }
        return this._selectResult(result);
    };
    SearchHelper.prototype._initLinesCache = function () {
        var _this = this;
        if (!this._linesCache) {
            this._linesCache = new Array(this._terminal.buffer.length);
            this._cursorMoveListener = this._terminal.onCursorMove(function () { return _this._destroyLinesCache(); });
            this._resizeListener = this._terminal.onResize(function () { return _this._destroyLinesCache(); });
        }
        window.clearTimeout(this._linesCacheTimeoutId);
        this._linesCacheTimeoutId = window.setTimeout(function () { return _this._destroyLinesCache(); }, LINES_CACHE_TIME_TO_LIVE);
    };
    SearchHelper.prototype._destroyLinesCache = function () {
        this._linesCache = null;
        if (this._cursorMoveListener) {
            this._cursorMoveListener.dispose();
            this._cursorMoveListener = undefined;
        }
        if (this._resizeListener) {
            this._resizeListener.dispose();
            this._resizeListener = undefined;
        }
        if (this._linesCacheTimeoutId) {
            window.clearTimeout(this._linesCacheTimeoutId);
            this._linesCacheTimeoutId = 0;
        }
    };
    SearchHelper.prototype._isWholeWord = function (searchIndex, line, term) {
        return (((searchIndex === 0) || (NON_WORD_CHARACTERS.indexOf(line[searchIndex - 1]) !== -1)) &&
            (((searchIndex + term.length) === line.length) || (NON_WORD_CHARACTERS.indexOf(line[searchIndex + term.length]) !== -1)));
    };
    SearchHelper.prototype._findInLine = function (term, row, col, searchOptions, isReverseSearch) {
        if (searchOptions === void 0) { searchOptions = {}; }
        if (isReverseSearch === void 0) { isReverseSearch = false; }
        if (this._terminal.buffer.getLine(row).isWrapped) {
            return;
        }
        var stringLine = this._linesCache ? this._linesCache[row] : void 0;
        if (stringLine === void 0) {
            stringLine = this.translateBufferLineToStringWithWrap(row, true);
            if (this._linesCache) {
                this._linesCache[row] = stringLine;
            }
        }
        var searchTerm = searchOptions.caseSensitive ? term : term.toLowerCase();
        var searchStringLine = searchOptions.caseSensitive ? stringLine : stringLine.toLowerCase();
        var resultIndex = -1;
        if (searchOptions.regex) {
            var searchRegex = RegExp(searchTerm, 'g');
            var foundTerm = void 0;
            if (isReverseSearch) {
                while (foundTerm = searchRegex.exec(searchStringLine.slice(0, col))) {
                    resultIndex = searchRegex.lastIndex - foundTerm[0].length;
                    term = foundTerm[0];
                    searchRegex.lastIndex -= (term.length - 1);
                }
            }
            else {
                foundTerm = searchRegex.exec(searchStringLine.slice(col));
                if (foundTerm && foundTerm[0].length > 0) {
                    resultIndex = col + (searchRegex.lastIndex - foundTerm[0].length);
                    term = foundTerm[0];
                }
            }
        }
        else {
            if (isReverseSearch) {
                if (col - searchTerm.length >= 0) {
                    resultIndex = searchStringLine.lastIndexOf(searchTerm, col - searchTerm.length);
                }
            }
            else {
                resultIndex = searchStringLine.indexOf(searchTerm, col);
            }
        }
        if (resultIndex >= 0) {
            if (resultIndex >= this._terminal.cols) {
                row += Math.floor(resultIndex / this._terminal.cols);
                resultIndex = resultIndex % this._terminal.cols;
            }
            if (searchOptions.wholeWord && !this._isWholeWord(resultIndex, searchStringLine, term)) {
                return;
            }
            var line = this._terminal.buffer.getLine(row);
            for (var i = 0; i < resultIndex; i++) {
                var cell = line.getCell(i);
                var char = cell.char;
                if (char.length > 1) {
                    resultIndex -= char.length - 1;
                }
                var charWidth = cell.width;
                if (charWidth === 0) {
                    resultIndex++;
                }
            }
            return {
                term: term,
                col: resultIndex,
                row: row
            };
        }
    };
    SearchHelper.prototype.translateBufferLineToStringWithWrap = function (lineIndex, trimRight) {
        var lineString = '';
        var lineWrapsToNext;
        do {
            var nextLine = this._terminal.buffer.getLine(lineIndex + 1);
            lineWrapsToNext = nextLine ? nextLine.isWrapped : false;
            lineString += this._terminal.buffer.getLine(lineIndex).translateToString(!lineWrapsToNext && trimRight).substring(0, this._terminal.cols);
            lineIndex++;
        } while (lineWrapsToNext);
        return lineString;
    };
    SearchHelper.prototype._selectResult = function (result) {
        if (!result) {
            this._terminal.clearSelection();
            return false;
        }
        this._terminal.select(result.col, result.row, result.term.length);
        this._terminal.scrollLines(result.row - this._terminal.buffer.viewportY);
        return true;
    };
    return SearchHelper;
}());
exports.SearchHelper = SearchHelper;

},{}],2:[function(require,module,exports){
"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var SearchHelper_1 = require("./SearchHelper");
function findNext(terminal, term, searchOptions) {
    if (searchOptions === void 0) { searchOptions = {}; }
    var addonTerminal = terminal;
    if (!addonTerminal.__searchHelper) {
        addonTerminal.__searchHelper = new SearchHelper_1.SearchHelper(addonTerminal);
    }
    return addonTerminal.__searchHelper.findNext(term, searchOptions);
}
exports.findNext = findNext;
function findPrevious(terminal, term, searchOptions) {
    var addonTerminal = terminal;
    if (!addonTerminal.__searchHelper) {
        addonTerminal.__searchHelper = new SearchHelper_1.SearchHelper(addonTerminal);
    }
    return addonTerminal.__searchHelper.findPrevious(term, searchOptions);
}
exports.findPrevious = findPrevious;
function apply(terminalConstructor) {
    terminalConstructor.prototype.findNext = function (term, searchOptions) {
        return findNext(this, term, searchOptions);
    };
    terminalConstructor.prototype.findPrevious = function (term, searchOptions) {
        return findPrevious(this, term, searchOptions);
    };
}
exports.apply = apply;

},{"./SearchHelper":1}]},{},[2])(2)
});
//# sourceMappingURL=search.js.map
