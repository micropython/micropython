"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var protocolClause = '(https?:\\/\\/)';
var domainCharacterSet = '[\\da-z\\.-]+';
var negatedDomainCharacterSet = '[^\\da-z\\.-]+';
var domainBodyClause = '(' + domainCharacterSet + ')';
var tldClause = '([a-z\\.]{2,6})';
var ipClause = '((\\d{1,3}\\.){3}\\d{1,3})';
var localHostClause = '(localhost)';
var portClause = '(:\\d{1,5})';
var hostClause = '((' + domainBodyClause + '\\.' + tldClause + ')|' + ipClause + '|' + localHostClause + ')' + portClause + '?';
var pathCharacterSet = '(\\/[\\/\\w\\.\\-%~:+]*)*([^:"\'\\s])';
var pathClause = '(' + pathCharacterSet + ')?';
var queryStringHashFragmentCharacterSet = '[0-9\\w\\[\\]\\(\\)\\/\\?\\!#@$%&\'*+,:;~\\=\\.\\-]*';
var queryStringClause = '(\\?' + queryStringHashFragmentCharacterSet + ')?';
var hashFragmentClause = '(#' + queryStringHashFragmentCharacterSet + ')?';
var negatedPathCharacterSet = '[^\\/\\w\\.\\-%]+';
var bodyClause = hostClause + pathClause + queryStringClause + hashFragmentClause;
var start = '(?:^|' + negatedDomainCharacterSet + ')(';
var end = ')($|' + negatedPathCharacterSet + ')';
var strictUrlRegex = new RegExp(start + protocolClause + bodyClause + end);
function handleLink(event, uri) {
    window.open(uri, '_blank');
}
function webLinksInit(term, handler, options) {
    if (handler === void 0) { handler = handleLink; }
    if (options === void 0) { options = {}; }
    options.matchIndex = 1;
    term.registerLinkMatcher(strictUrlRegex, handler, options);
}
exports.webLinksInit = webLinksInit;
function apply(terminalConstructor) {
    terminalConstructor.prototype.webLinksInit = function (handler, options) {
        webLinksInit(this, handler, options);
    };
}
exports.apply = apply;
//# sourceMappingURL=webLinks.js.map