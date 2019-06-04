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
//# sourceMappingURL=search.js.map