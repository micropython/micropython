"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
function prepareTextForTerminal(text) {
    return text.replace(/\r?\n/g, '\r');
}
exports.prepareTextForTerminal = prepareTextForTerminal;
function bracketTextForPaste(text, bracketedPasteMode) {
    if (bracketedPasteMode) {
        return '\x1b[200~' + text + '\x1b[201~';
    }
    return text;
}
exports.bracketTextForPaste = bracketTextForPaste;
function copyHandler(ev, term, selectionManager) {
    if (term.browser.isMSIE) {
        window.clipboardData.setData('Text', selectionManager.selectionText);
    }
    else {
        ev.clipboardData.setData('text/plain', selectionManager.selectionText);
    }
    ev.preventDefault();
}
exports.copyHandler = copyHandler;
function pasteHandler(ev, term) {
    ev.stopPropagation();
    var text;
    var dispatchPaste = function (text) {
        text = prepareTextForTerminal(text);
        text = bracketTextForPaste(text, term.bracketedPasteMode);
        term.handler(text);
        term.textarea.value = '';
        term.emit('paste', text);
        term.cancel(ev);
    };
    if (term.browser.isMSIE) {
        if (window.clipboardData) {
            text = window.clipboardData.getData('Text');
            dispatchPaste(text);
        }
    }
    else {
        if (ev.clipboardData) {
            text = ev.clipboardData.getData('text/plain');
            dispatchPaste(text);
        }
    }
}
exports.pasteHandler = pasteHandler;
function moveTextAreaUnderMouseCursor(ev, term) {
    var pos = term.screenElement.getBoundingClientRect();
    var left = ev.clientX - pos.left - 10;
    var top = ev.clientY - pos.top - 10;
    term.textarea.style.position = 'absolute';
    term.textarea.style.width = '20px';
    term.textarea.style.height = '20px';
    term.textarea.style.left = left + "px";
    term.textarea.style.top = top + "px";
    term.textarea.style.zIndex = '1000';
    term.textarea.focus();
    setTimeout(function () {
        term.textarea.style.position = null;
        term.textarea.style.width = null;
        term.textarea.style.height = null;
        term.textarea.style.left = null;
        term.textarea.style.top = null;
        term.textarea.style.zIndex = null;
    }, 200);
}
exports.moveTextAreaUnderMouseCursor = moveTextAreaUnderMouseCursor;
function rightClickHandler(ev, term, selectionManager, shouldSelectWord) {
    moveTextAreaUnderMouseCursor(ev, term);
    if (shouldSelectWord && !selectionManager.isClickInSelection(ev)) {
        selectionManager.selectWordAtCursor(ev);
    }
    term.textarea.value = selectionManager.selectionText;
    term.textarea.select();
}
exports.rightClickHandler = rightClickHandler;
//# sourceMappingURL=Clipboard.js.map