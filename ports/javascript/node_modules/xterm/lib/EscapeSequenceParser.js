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
var Lifecycle_1 = require("./common/Lifecycle");
var TextDecoder_1 = require("./core/input/TextDecoder");
function r(low, high) {
    var c = high - low;
    var arr = new Array(c);
    while (c--) {
        arr[c] = --high;
    }
    return arr;
}
var TransitionTable = (function () {
    function TransitionTable(length) {
        this.table = (typeof Uint8Array === 'undefined')
            ? new Array(length)
            : new Uint8Array(length);
    }
    TransitionTable.prototype.add = function (code, state, action, next) {
        this.table[state << 8 | code] = ((action | 0) << 4) | ((next === undefined) ? state : next);
    };
    TransitionTable.prototype.addMany = function (codes, state, action, next) {
        for (var i = 0; i < codes.length; i++) {
            this.add(codes[i], state, action, next);
        }
    };
    return TransitionTable;
}());
exports.TransitionTable = TransitionTable;
var PRINTABLES = r(0x20, 0x7f);
var EXECUTABLES = r(0x00, 0x18);
EXECUTABLES.push(0x19);
EXECUTABLES.push.apply(EXECUTABLES, r(0x1c, 0x20));
var NON_ASCII_PRINTABLE = 0xA0;
exports.VT500_TRANSITION_TABLE = (function () {
    var table = new TransitionTable(4095);
    var states = r(0, 13 + 1);
    var state;
    for (state in states) {
        for (var code = 0; code <= NON_ASCII_PRINTABLE; ++code) {
            table.add(code, state, 1, 0);
        }
    }
    table.addMany(PRINTABLES, 0, 2, 0);
    for (state in states) {
        table.addMany([0x18, 0x1a, 0x99, 0x9a], state, 3, 0);
        table.addMany(r(0x80, 0x90), state, 3, 0);
        table.addMany(r(0x90, 0x98), state, 3, 0);
        table.add(0x9c, state, 0, 0);
        table.add(0x1b, state, 11, 1);
        table.add(0x9d, state, 4, 8);
        table.addMany([0x98, 0x9e, 0x9f], state, 0, 7);
        table.add(0x9b, state, 11, 3);
        table.add(0x90, state, 11, 9);
    }
    table.addMany(EXECUTABLES, 0, 3, 0);
    table.addMany(EXECUTABLES, 1, 3, 1);
    table.add(0x7f, 1, 0, 1);
    table.addMany(EXECUTABLES, 8, 0, 8);
    table.addMany(EXECUTABLES, 3, 3, 3);
    table.add(0x7f, 3, 0, 3);
    table.addMany(EXECUTABLES, 4, 3, 4);
    table.add(0x7f, 4, 0, 4);
    table.addMany(EXECUTABLES, 6, 3, 6);
    table.addMany(EXECUTABLES, 5, 3, 5);
    table.add(0x7f, 5, 0, 5);
    table.addMany(EXECUTABLES, 2, 3, 2);
    table.add(0x7f, 2, 0, 2);
    table.add(0x5d, 1, 4, 8);
    table.addMany(PRINTABLES, 8, 5, 8);
    table.add(0x7f, 8, 5, 8);
    table.addMany([0x9c, 0x1b, 0x18, 0x1a, 0x07], 8, 6, 0);
    table.addMany(r(0x1c, 0x20), 8, 0, 8);
    table.addMany([0x58, 0x5e, 0x5f], 1, 0, 7);
    table.addMany(PRINTABLES, 7, 0, 7);
    table.addMany(EXECUTABLES, 7, 0, 7);
    table.add(0x9c, 7, 0, 0);
    table.add(0x7f, 7, 0, 7);
    table.add(0x5b, 1, 11, 3);
    table.addMany(r(0x40, 0x7f), 3, 7, 0);
    table.addMany(r(0x30, 0x3a), 3, 8, 4);
    table.add(0x3b, 3, 8, 4);
    table.addMany([0x3c, 0x3d, 0x3e, 0x3f], 3, 9, 4);
    table.addMany(r(0x30, 0x3a), 4, 8, 4);
    table.add(0x3b, 4, 8, 4);
    table.addMany(r(0x40, 0x7f), 4, 7, 0);
    table.addMany([0x3a, 0x3c, 0x3d, 0x3e, 0x3f], 4, 0, 6);
    table.addMany(r(0x20, 0x40), 6, 0, 6);
    table.add(0x7f, 6, 0, 6);
    table.addMany(r(0x40, 0x7f), 6, 0, 0);
    table.add(0x3a, 3, 0, 6);
    table.addMany(r(0x20, 0x30), 3, 9, 5);
    table.addMany(r(0x20, 0x30), 5, 9, 5);
    table.addMany(r(0x30, 0x40), 5, 0, 6);
    table.addMany(r(0x40, 0x7f), 5, 7, 0);
    table.addMany(r(0x20, 0x30), 4, 9, 5);
    table.addMany(r(0x20, 0x30), 1, 9, 2);
    table.addMany(r(0x20, 0x30), 2, 9, 2);
    table.addMany(r(0x30, 0x7f), 2, 10, 0);
    table.addMany(r(0x30, 0x50), 1, 10, 0);
    table.addMany(r(0x51, 0x58), 1, 10, 0);
    table.addMany([0x59, 0x5a, 0x5c], 1, 10, 0);
    table.addMany(r(0x60, 0x7f), 1, 10, 0);
    table.add(0x50, 1, 11, 9);
    table.addMany(EXECUTABLES, 9, 0, 9);
    table.add(0x7f, 9, 0, 9);
    table.addMany(r(0x1c, 0x20), 9, 0, 9);
    table.addMany(r(0x20, 0x30), 9, 9, 12);
    table.add(0x3a, 9, 0, 11);
    table.addMany(r(0x30, 0x3a), 9, 8, 10);
    table.add(0x3b, 9, 8, 10);
    table.addMany([0x3c, 0x3d, 0x3e, 0x3f], 9, 9, 10);
    table.addMany(EXECUTABLES, 11, 0, 11);
    table.addMany(r(0x20, 0x80), 11, 0, 11);
    table.addMany(r(0x1c, 0x20), 11, 0, 11);
    table.addMany(EXECUTABLES, 10, 0, 10);
    table.add(0x7f, 10, 0, 10);
    table.addMany(r(0x1c, 0x20), 10, 0, 10);
    table.addMany(r(0x30, 0x3a), 10, 8, 10);
    table.add(0x3b, 10, 8, 10);
    table.addMany([0x3a, 0x3c, 0x3d, 0x3e, 0x3f], 10, 0, 11);
    table.addMany(r(0x20, 0x30), 10, 9, 12);
    table.addMany(EXECUTABLES, 12, 0, 12);
    table.add(0x7f, 12, 0, 12);
    table.addMany(r(0x1c, 0x20), 12, 0, 12);
    table.addMany(r(0x20, 0x30), 12, 9, 12);
    table.addMany(r(0x30, 0x40), 12, 0, 11);
    table.addMany(r(0x40, 0x7f), 12, 12, 13);
    table.addMany(r(0x40, 0x7f), 10, 12, 13);
    table.addMany(r(0x40, 0x7f), 9, 12, 13);
    table.addMany(EXECUTABLES, 13, 13, 13);
    table.addMany(PRINTABLES, 13, 13, 13);
    table.add(0x7f, 13, 0, 13);
    table.addMany([0x1b, 0x9c], 13, 14, 0);
    table.add(NON_ASCII_PRINTABLE, 8, 5, 8);
    return table;
})();
var DcsDummy = (function () {
    function DcsDummy() {
    }
    DcsDummy.prototype.hook = function (collect, params, flag) { };
    DcsDummy.prototype.put = function (data, start, end) { };
    DcsDummy.prototype.unhook = function () { };
    return DcsDummy;
}());
var EscapeSequenceParser = (function (_super) {
    __extends(EscapeSequenceParser, _super);
    function EscapeSequenceParser(TRANSITIONS) {
        if (TRANSITIONS === void 0) { TRANSITIONS = exports.VT500_TRANSITION_TABLE; }
        var _this = _super.call(this) || this;
        _this.TRANSITIONS = TRANSITIONS;
        _this.initialState = 0;
        _this.currentState = _this.initialState;
        _this._osc = '';
        _this._params = [0];
        _this._collect = '';
        _this._printHandlerFb = function (data, start, end) { };
        _this._executeHandlerFb = function (code) { };
        _this._csiHandlerFb = function (collect, params, flag) { };
        _this._escHandlerFb = function (collect, flag) { };
        _this._oscHandlerFb = function (identifier, data) { };
        _this._dcsHandlerFb = new DcsDummy();
        _this._errorHandlerFb = function (state) { return state; };
        _this._printHandler = _this._printHandlerFb;
        _this._executeHandlers = Object.create(null);
        _this._csiHandlers = Object.create(null);
        _this._escHandlers = Object.create(null);
        _this._oscHandlers = Object.create(null);
        _this._dcsHandlers = Object.create(null);
        _this._activeDcsHandler = null;
        _this._errorHandler = _this._errorHandlerFb;
        _this.setEscHandler('\\', function () { });
        return _this;
    }
    EscapeSequenceParser.prototype.dispose = function () {
        this._printHandlerFb = null;
        this._executeHandlerFb = null;
        this._csiHandlerFb = null;
        this._escHandlerFb = null;
        this._oscHandlerFb = null;
        this._dcsHandlerFb = null;
        this._errorHandlerFb = null;
        this._printHandler = null;
        this._executeHandlers = null;
        this._escHandlers = null;
        this._csiHandlers = null;
        this._oscHandlers = null;
        this._dcsHandlers = null;
        this._activeDcsHandler = null;
        this._errorHandler = null;
    };
    EscapeSequenceParser.prototype.setPrintHandler = function (callback) {
        this._printHandler = callback;
    };
    EscapeSequenceParser.prototype.clearPrintHandler = function () {
        this._printHandler = this._printHandlerFb;
    };
    EscapeSequenceParser.prototype.setExecuteHandler = function (flag, callback) {
        this._executeHandlers[flag.charCodeAt(0)] = callback;
    };
    EscapeSequenceParser.prototype.clearExecuteHandler = function (flag) {
        if (this._executeHandlers[flag.charCodeAt(0)])
            delete this._executeHandlers[flag.charCodeAt(0)];
    };
    EscapeSequenceParser.prototype.setExecuteHandlerFallback = function (callback) {
        this._executeHandlerFb = callback;
    };
    EscapeSequenceParser.prototype.addCsiHandler = function (flag, callback) {
        var index = flag.charCodeAt(0);
        if (this._csiHandlers[index] === undefined) {
            this._csiHandlers[index] = [];
        }
        var handlerList = this._csiHandlers[index];
        handlerList.push(callback);
        return {
            dispose: function () {
                var handlerIndex = handlerList.indexOf(callback);
                if (handlerIndex !== -1) {
                    handlerList.splice(handlerIndex, 1);
                }
            }
        };
    };
    EscapeSequenceParser.prototype.setCsiHandler = function (flag, callback) {
        this._csiHandlers[flag.charCodeAt(0)] = [callback];
    };
    EscapeSequenceParser.prototype.clearCsiHandler = function (flag) {
        if (this._csiHandlers[flag.charCodeAt(0)])
            delete this._csiHandlers[flag.charCodeAt(0)];
    };
    EscapeSequenceParser.prototype.setCsiHandlerFallback = function (callback) {
        this._csiHandlerFb = callback;
    };
    EscapeSequenceParser.prototype.setEscHandler = function (collectAndFlag, callback) {
        this._escHandlers[collectAndFlag] = callback;
    };
    EscapeSequenceParser.prototype.clearEscHandler = function (collectAndFlag) {
        if (this._escHandlers[collectAndFlag])
            delete this._escHandlers[collectAndFlag];
    };
    EscapeSequenceParser.prototype.setEscHandlerFallback = function (callback) {
        this._escHandlerFb = callback;
    };
    EscapeSequenceParser.prototype.addOscHandler = function (ident, callback) {
        if (this._oscHandlers[ident] === undefined) {
            this._oscHandlers[ident] = [];
        }
        var handlerList = this._oscHandlers[ident];
        handlerList.push(callback);
        return {
            dispose: function () {
                var handlerIndex = handlerList.indexOf(callback);
                if (handlerIndex !== -1) {
                    handlerList.splice(handlerIndex, 1);
                }
            }
        };
    };
    EscapeSequenceParser.prototype.setOscHandler = function (ident, callback) {
        this._oscHandlers[ident] = [callback];
    };
    EscapeSequenceParser.prototype.clearOscHandler = function (ident) {
        if (this._oscHandlers[ident])
            delete this._oscHandlers[ident];
    };
    EscapeSequenceParser.prototype.setOscHandlerFallback = function (callback) {
        this._oscHandlerFb = callback;
    };
    EscapeSequenceParser.prototype.setDcsHandler = function (collectAndFlag, handler) {
        this._dcsHandlers[collectAndFlag] = handler;
    };
    EscapeSequenceParser.prototype.clearDcsHandler = function (collectAndFlag) {
        if (this._dcsHandlers[collectAndFlag])
            delete this._dcsHandlers[collectAndFlag];
    };
    EscapeSequenceParser.prototype.setDcsHandlerFallback = function (handler) {
        this._dcsHandlerFb = handler;
    };
    EscapeSequenceParser.prototype.setErrorHandler = function (callback) {
        this._errorHandler = callback;
    };
    EscapeSequenceParser.prototype.clearErrorHandler = function () {
        this._errorHandler = this._errorHandlerFb;
    };
    EscapeSequenceParser.prototype.reset = function () {
        this.currentState = this.initialState;
        this._osc = '';
        this._params = [0];
        this._collect = '';
        this._activeDcsHandler = null;
    };
    EscapeSequenceParser.prototype.parse = function (data, length) {
        var code = 0;
        var transition = 0;
        var error = false;
        var currentState = this.currentState;
        var print = -1;
        var dcs = -1;
        var osc = this._osc;
        var collect = this._collect;
        var params = this._params;
        var table = this.TRANSITIONS.table;
        var dcsHandler = this._activeDcsHandler;
        var callback = null;
        for (var i = 0; i < length; ++i) {
            code = data[i];
            if (currentState === 0 && code > 0x1f && code < 0x80) {
                print = (~print) ? print : i;
                do
                    i++;
                while (i < length && data[i] > 0x1f && data[i] < 0x80);
                i--;
                continue;
            }
            if (currentState === 4 && (code > 0x2f && code < 0x39)) {
                params[params.length - 1] = params[params.length - 1] * 10 + code - 48;
                continue;
            }
            transition = table[currentState << 8 | (code < 0xa0 ? code : NON_ASCII_PRINTABLE)];
            switch (transition >> 4) {
                case 2:
                    print = (~print) ? print : i;
                    break;
                case 3:
                    if (~print) {
                        this._printHandler(data, print, i);
                        print = -1;
                    }
                    callback = this._executeHandlers[code];
                    if (callback)
                        callback();
                    else
                        this._executeHandlerFb(code);
                    break;
                case 0:
                    if (~print) {
                        this._printHandler(data, print, i);
                        print = -1;
                    }
                    else if (~dcs) {
                        dcsHandler.put(data, dcs, i);
                        dcs = -1;
                    }
                    break;
                case 1:
                    if (code > 0x9f) {
                        switch (currentState) {
                            case 0:
                                print = (~print) ? print : i;
                                break;
                            case 6:
                                transition |= 6;
                                break;
                            case 11:
                                transition |= 11;
                                break;
                            case 13:
                                dcs = (~dcs) ? dcs : i;
                                transition |= 13;
                                break;
                            default:
                                error = true;
                        }
                    }
                    else {
                        error = true;
                    }
                    if (error) {
                        var inject = this._errorHandler({
                            position: i,
                            code: code,
                            currentState: currentState,
                            print: print,
                            dcs: dcs,
                            osc: osc,
                            collect: collect,
                            params: params,
                            abort: false
                        });
                        if (inject.abort)
                            return;
                        error = false;
                    }
                    break;
                case 7:
                    var handlers = this._csiHandlers[code];
                    var j = handlers ? handlers.length - 1 : -1;
                    for (; j >= 0; j--) {
                        if (handlers[j](params, collect) !== false) {
                            break;
                        }
                    }
                    if (j < 0) {
                        this._csiHandlerFb(collect, params, code);
                    }
                    break;
                case 8:
                    if (code === 0x3b)
                        params.push(0);
                    else
                        params[params.length - 1] = params[params.length - 1] * 10 + code - 48;
                    break;
                case 9:
                    collect += String.fromCharCode(code);
                    break;
                case 10:
                    callback = this._escHandlers[collect + String.fromCharCode(code)];
                    if (callback)
                        callback(collect, code);
                    else
                        this._escHandlerFb(collect, code);
                    break;
                case 11:
                    if (~print) {
                        this._printHandler(data, print, i);
                        print = -1;
                    }
                    osc = '';
                    params = [0];
                    collect = '';
                    dcs = -1;
                    break;
                case 12:
                    dcsHandler = this._dcsHandlers[collect + String.fromCharCode(code)];
                    if (!dcsHandler)
                        dcsHandler = this._dcsHandlerFb;
                    dcsHandler.hook(collect, params, code);
                    break;
                case 13:
                    dcs = (~dcs) ? dcs : i;
                    break;
                case 14:
                    if (dcsHandler) {
                        if (~dcs)
                            dcsHandler.put(data, dcs, i);
                        dcsHandler.unhook();
                        dcsHandler = null;
                    }
                    if (code === 0x1b)
                        transition |= 1;
                    osc = '';
                    params = [0];
                    collect = '';
                    dcs = -1;
                    break;
                case 4:
                    if (~print) {
                        this._printHandler(data, print, i);
                        print = -1;
                    }
                    osc = '';
                    break;
                case 5:
                    for (var j_1 = i + 1;; j_1++) {
                        if (j_1 >= length
                            || (code = data[j_1]) < 0x20
                            || (code > 0x7f && code <= 0x9f)) {
                            osc += TextDecoder_1.utf32ToString(data, i, j_1);
                            i = j_1 - 1;
                            break;
                        }
                    }
                    break;
                case 6:
                    if (osc && code !== 0x18 && code !== 0x1a) {
                        var idx = osc.indexOf(';');
                        if (idx === -1) {
                            this._oscHandlerFb(-1, osc);
                        }
                        else {
                            var identifier = parseInt(osc.substring(0, idx));
                            var content = osc.substring(idx + 1);
                            var handlers_1 = this._oscHandlers[identifier];
                            var j_2 = handlers_1 ? handlers_1.length - 1 : -1;
                            for (; j_2 >= 0; j_2--) {
                                if (handlers_1[j_2](content) !== false) {
                                    break;
                                }
                            }
                            if (j_2 < 0) {
                                this._oscHandlerFb(identifier, content);
                            }
                        }
                    }
                    if (code === 0x1b)
                        transition |= 1;
                    osc = '';
                    params = [0];
                    collect = '';
                    dcs = -1;
                    break;
            }
            currentState = transition & 15;
        }
        if (currentState === 0 && ~print) {
            this._printHandler(data, print, length);
        }
        else if (currentState === 13 && ~dcs && dcsHandler) {
            dcsHandler.put(data, dcs, length);
        }
        this._osc = osc;
        this._collect = collect;
        this._params = params;
        this._activeDcsHandler = dcsHandler;
        this.currentState = currentState;
    };
    return EscapeSequenceParser;
}(Lifecycle_1.Disposable));
exports.EscapeSequenceParser = EscapeSequenceParser;
//# sourceMappingURL=EscapeSequenceParser.js.map