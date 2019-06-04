"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
var EscapeSequences_1 = require("../../common/data/EscapeSequences");
var KEYCODE_KEY_MAPPINGS = {
    48: ['0', ')'],
    49: ['1', '!'],
    50: ['2', '@'],
    51: ['3', '#'],
    52: ['4', '$'],
    53: ['5', '%'],
    54: ['6', '^'],
    55: ['7', '&'],
    56: ['8', '*'],
    57: ['9', '('],
    186: [';', ':'],
    187: ['=', '+'],
    188: [',', '<'],
    189: ['-', '_'],
    190: ['.', '>'],
    191: ['/', '?'],
    192: ['`', '~'],
    219: ['[', '{'],
    220: ['\\', '|'],
    221: [']', '}'],
    222: ['\'', '"']
};
function evaluateKeyboardEvent(ev, applicationCursorMode, isMac, macOptionIsMeta) {
    var result = {
        type: 0,
        cancel: false,
        key: undefined
    };
    var modifiers = (ev.shiftKey ? 1 : 0) | (ev.altKey ? 2 : 0) | (ev.ctrlKey ? 4 : 0) | (ev.metaKey ? 8 : 0);
    switch (ev.keyCode) {
        case 0:
            if (ev.key === 'UIKeyInputUpArrow') {
                if (applicationCursorMode) {
                    result.key = EscapeSequences_1.C0.ESC + 'OA';
                }
                else {
                    result.key = EscapeSequences_1.C0.ESC + '[A';
                }
            }
            else if (ev.key === 'UIKeyInputLeftArrow') {
                if (applicationCursorMode) {
                    result.key = EscapeSequences_1.C0.ESC + 'OD';
                }
                else {
                    result.key = EscapeSequences_1.C0.ESC + '[D';
                }
            }
            else if (ev.key === 'UIKeyInputRightArrow') {
                if (applicationCursorMode) {
                    result.key = EscapeSequences_1.C0.ESC + 'OC';
                }
                else {
                    result.key = EscapeSequences_1.C0.ESC + '[C';
                }
            }
            else if (ev.key === 'UIKeyInputDownArrow') {
                if (applicationCursorMode) {
                    result.key = EscapeSequences_1.C0.ESC + 'OB';
                }
                else {
                    result.key = EscapeSequences_1.C0.ESC + '[B';
                }
            }
            break;
        case 8:
            if (ev.shiftKey) {
                result.key = EscapeSequences_1.C0.BS;
                break;
            }
            else if (ev.altKey) {
                result.key = EscapeSequences_1.C0.ESC + EscapeSequences_1.C0.DEL;
                break;
            }
            result.key = EscapeSequences_1.C0.DEL;
            break;
        case 9:
            if (ev.shiftKey) {
                result.key = EscapeSequences_1.C0.ESC + '[Z';
                break;
            }
            result.key = EscapeSequences_1.C0.HT;
            result.cancel = true;
            break;
        case 13:
            result.key = EscapeSequences_1.C0.CR;
            result.cancel = true;
            break;
        case 27:
            result.key = EscapeSequences_1.C0.ESC;
            result.cancel = true;
            break;
        case 37:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[1;' + (modifiers + 1) + 'D';
                if (result.key === EscapeSequences_1.C0.ESC + '[1;3D') {
                    result.key = isMac ? EscapeSequences_1.C0.ESC + 'b' : EscapeSequences_1.C0.ESC + '[1;5D';
                }
            }
            else if (applicationCursorMode) {
                result.key = EscapeSequences_1.C0.ESC + 'OD';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[D';
            }
            break;
        case 39:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[1;' + (modifiers + 1) + 'C';
                if (result.key === EscapeSequences_1.C0.ESC + '[1;3C') {
                    result.key = isMac ? EscapeSequences_1.C0.ESC + 'f' : EscapeSequences_1.C0.ESC + '[1;5C';
                }
            }
            else if (applicationCursorMode) {
                result.key = EscapeSequences_1.C0.ESC + 'OC';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[C';
            }
            break;
        case 38:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[1;' + (modifiers + 1) + 'A';
                if (result.key === EscapeSequences_1.C0.ESC + '[1;3A') {
                    result.key = EscapeSequences_1.C0.ESC + '[1;5A';
                }
            }
            else if (applicationCursorMode) {
                result.key = EscapeSequences_1.C0.ESC + 'OA';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[A';
            }
            break;
        case 40:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[1;' + (modifiers + 1) + 'B';
                if (result.key === EscapeSequences_1.C0.ESC + '[1;3B') {
                    result.key = EscapeSequences_1.C0.ESC + '[1;5B';
                }
            }
            else if (applicationCursorMode) {
                result.key = EscapeSequences_1.C0.ESC + 'OB';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[B';
            }
            break;
        case 45:
            if (!ev.shiftKey && !ev.ctrlKey) {
                result.key = EscapeSequences_1.C0.ESC + '[2~';
            }
            break;
        case 46:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[3;' + (modifiers + 1) + '~';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[3~';
            }
            break;
        case 36:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[1;' + (modifiers + 1) + 'H';
            }
            else if (applicationCursorMode) {
                result.key = EscapeSequences_1.C0.ESC + 'OH';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[H';
            }
            break;
        case 35:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[1;' + (modifiers + 1) + 'F';
            }
            else if (applicationCursorMode) {
                result.key = EscapeSequences_1.C0.ESC + 'OF';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[F';
            }
            break;
        case 33:
            if (ev.shiftKey) {
                result.type = 2;
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[5~';
            }
            break;
        case 34:
            if (ev.shiftKey) {
                result.type = 3;
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[6~';
            }
            break;
        case 112:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[1;' + (modifiers + 1) + 'P';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + 'OP';
            }
            break;
        case 113:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[1;' + (modifiers + 1) + 'Q';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + 'OQ';
            }
            break;
        case 114:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[1;' + (modifiers + 1) + 'R';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + 'OR';
            }
            break;
        case 115:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[1;' + (modifiers + 1) + 'S';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + 'OS';
            }
            break;
        case 116:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[15;' + (modifiers + 1) + '~';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[15~';
            }
            break;
        case 117:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[17;' + (modifiers + 1) + '~';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[17~';
            }
            break;
        case 118:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[18;' + (modifiers + 1) + '~';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[18~';
            }
            break;
        case 119:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[19;' + (modifiers + 1) + '~';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[19~';
            }
            break;
        case 120:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[20;' + (modifiers + 1) + '~';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[20~';
            }
            break;
        case 121:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[21;' + (modifiers + 1) + '~';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[21~';
            }
            break;
        case 122:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[23;' + (modifiers + 1) + '~';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[23~';
            }
            break;
        case 123:
            if (modifiers) {
                result.key = EscapeSequences_1.C0.ESC + '[24;' + (modifiers + 1) + '~';
            }
            else {
                result.key = EscapeSequences_1.C0.ESC + '[24~';
            }
            break;
        default:
            if (ev.ctrlKey && !ev.shiftKey && !ev.altKey && !ev.metaKey) {
                if (ev.keyCode >= 65 && ev.keyCode <= 90) {
                    result.key = String.fromCharCode(ev.keyCode - 64);
                }
                else if (ev.keyCode === 32) {
                    result.key = String.fromCharCode(0);
                }
                else if (ev.keyCode >= 51 && ev.keyCode <= 55) {
                    result.key = String.fromCharCode(ev.keyCode - 51 + 27);
                }
                else if (ev.keyCode === 56) {
                    result.key = String.fromCharCode(127);
                }
                else if (ev.keyCode === 219) {
                    result.key = String.fromCharCode(27);
                }
                else if (ev.keyCode === 220) {
                    result.key = String.fromCharCode(28);
                }
                else if (ev.keyCode === 221) {
                    result.key = String.fromCharCode(29);
                }
            }
            else if ((!isMac || macOptionIsMeta) && ev.altKey && !ev.metaKey) {
                var keyMapping = KEYCODE_KEY_MAPPINGS[ev.keyCode];
                var key = keyMapping && keyMapping[!ev.shiftKey ? 0 : 1];
                if (key) {
                    result.key = EscapeSequences_1.C0.ESC + key;
                }
                else if (ev.keyCode >= 65 && ev.keyCode <= 90) {
                    var keyCode = ev.ctrlKey ? ev.keyCode - 64 : ev.keyCode + 32;
                    result.key = EscapeSequences_1.C0.ESC + String.fromCharCode(keyCode);
                }
            }
            else if (isMac && !ev.altKey && !ev.ctrlKey && ev.metaKey) {
                if (ev.keyCode === 65) {
                    result.type = 1;
                }
            }
            else if (ev.key && !ev.ctrlKey && !ev.altKey && !ev.metaKey && ev.keyCode >= 48 && ev.key.length === 1) {
                result.key = ev.key;
            }
            else if (ev.key && ev.ctrlKey) {
                if (ev.key === '_') {
                    result.key = EscapeSequences_1.C0.US;
                }
            }
            break;
    }
    return result;
}
exports.evaluateKeyboardEvent = evaluateKeyboardEvent;
//# sourceMappingURL=Keyboard.js.map