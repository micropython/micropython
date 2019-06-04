"use strict";
var __awaiter = (this && this.__awaiter) || function (thisArg, _arguments, P, generator) {
    return new (P || (P = Promise))(function (resolve, reject) {
        function fulfilled(value) { try { step(generator.next(value)); } catch (e) { reject(e); } }
        function rejected(value) { try { step(generator["throw"](value)); } catch (e) { reject(e); } }
        function step(result) { result.done ? resolve(result.value) : new P(function (resolve) { resolve(result.value); }).then(fulfilled, rejected); }
        step((generator = generator.apply(thisArg, _arguments || [])).next());
    });
};
var __generator = (this && this.__generator) || function (thisArg, body) {
    var _ = { label: 0, sent: function() { if (t[0] & 1) throw t[1]; return t[1]; }, trys: [], ops: [] }, f, y, t, g;
    return g = { next: verb(0), "throw": verb(1), "return": verb(2) }, typeof Symbol === "function" && (g[Symbol.iterator] = function() { return this; }), g;
    function verb(n) { return function (v) { return step([n, v]); }; }
    function step(op) {
        if (f) throw new TypeError("Generator is already executing.");
        while (_) try {
            if (f = 1, y && (t = op[0] & 2 ? y["return"] : op[0] ? y["throw"] || ((t = y["return"]) && t.call(y), 0) : y.next) && !(t = t.call(y, op[1])).done) return t;
            if (y = 0, t) op = [op[0] & 2, t.value];
            switch (op[0]) {
                case 0: case 1: t = op; break;
                case 4: _.label++; return { value: op[1], done: false };
                case 5: _.label++; y = op[1]; op = [0]; continue;
                case 7: op = _.ops.pop(); _.trys.pop(); continue;
                default:
                    if (!(t = _.trys, t = t.length > 0 && t[t.length - 1]) && (op[0] === 6 || op[0] === 2)) { _ = 0; continue; }
                    if (op[0] === 3 && (!t || (op[1] > t[0] && op[1] < t[3]))) { _.label = op[1]; break; }
                    if (op[0] === 6 && _.label < t[1]) { _.label = t[1]; t = op; break; }
                    if (t && _.label < t[2]) { _.label = t[2]; _.ops.push(op); break; }
                    if (t[2]) _.ops.pop();
                    _.trys.pop(); continue;
            }
            op = body.call(thisArg, _);
        } catch (e) { op = [6, e]; y = 0; } finally { f = t = 0; }
        if (op[0] & 5) throw op[1]; return { value: op[0] ? op[1] : void 0, done: true };
    }
};
Object.defineProperty(exports, "__esModule", { value: true });
var puppeteer = require("puppeteer");
var chai_1 = require("chai");
var APP = 'http://127.0.0.1:3000/test';
var browser;
var page;
var width = 800;
var height = 600;
describe('InputHandler Integration Tests', function () {
    var _this = this;
    this.timeout(20000);
    before(function () {
        return __awaiter(this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4, puppeteer.launch({
                            headless: process.argv.indexOf('--headless') !== -1,
                            slowMo: 80,
                            args: ["--window-size=" + width + "," + height]
                        })];
                    case 1:
                        browser = _a.sent();
                        return [4, browser.pages()];
                    case 2:
                        page = (_a.sent())[0];
                        return [4, page.setViewport({ width: width, height: height })];
                    case 3:
                        _a.sent();
                        return [4, page.goto(APP)];
                    case 4:
                        _a.sent();
                        return [4, openTerminal()];
                    case 5:
                        _a.sent();
                        return [2];
                }
            });
        });
    });
    after(function () {
        browser.close();
    });
    describe('CSI', function () {
        beforeEach(function () { return __awaiter(_this, void 0, void 0, function () {
            return __generator(this, function (_a) {
                switch (_a.label) {
                    case 0: return [4, page.evaluate("window.term.reset()")];
                    case 1:
                        _a.sent();
                        return [2];
                }
            });
        }); });
        it('ICH: Insert Ps (Blank) Character(s) (default = 1) - CSI Ps @', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0: return [4, page.evaluate("\n        // Default\n        window.term.write('foo\\x1b[3D\\x1b[@\\n\\r')\n        // Explicit\n        window.term.write('bar\\x1b[3D\\x1b[4@')\n      ")];
                        case 1:
                            _c.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(2)];
                        case 2:
                            _b.apply(_a, [_c.sent(), [' foo', '    bar']]);
                            return [2];
                    }
                });
            });
        });
        it('CUU: Cursor Up Ps Times (default = 1) - CSI Ps A', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0: return [4, page.evaluate("\n        // Default\n        window.term.write('\\n\\n\\n\\n\u001B[Aa')\n        // Explicit\n        window.term.write('\u001B[2Ab')\n      ")];
                        case 1:
                            _c.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(4)];
                        case 2:
                            _b.apply(_a, [_c.sent(), ['', ' b', '', 'a']]);
                            return [2];
                    }
                });
            });
        });
        it('CUD: Cursor Down Ps Times (default = 1) - CSI Ps B', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0: return [4, page.evaluate("\n        // Default\n        window.term.write('\u001B[Ba')\n        // Explicit\n        window.term.write('\u001B[2Bb')\n      ")];
                        case 1:
                            _c.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(4)];
                        case 2:
                            _b.apply(_a, [_c.sent(), ['', 'a', '', ' b']]);
                            return [2];
                    }
                });
            });
        });
        it('CUF: Cursor Forward Ps Times (default = 1) - CSI Ps C', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0: return [4, page.evaluate("\n        // Default\n        window.term.write('\u001B[Ca')\n        // Explicit\n        window.term.write('\u001B[2Cb')\n      ")];
                        case 1:
                            _c.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(1)];
                        case 2:
                            _b.apply(_a, [_c.sent(), [' a  b']]);
                            return [2];
                    }
                });
            });
        });
        it('CUB: Cursor Backward Ps Times (default = 1) - CSI Ps D', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0: return [4, page.evaluate("\n        // Default\n        window.term.write('foo\u001B[Da')\n        // Explicit\n        window.term.write('\u001B[2Db')\n      ")];
                        case 1:
                            _c.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(1)];
                        case 2:
                            _b.apply(_a, [_c.sent(), ['fba']]);
                            return [2];
                    }
                });
            });
        });
        it('CNL: Cursor Next Line Ps Times (default = 1) - CSI Ps E', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0: return [4, page.evaluate("\n        // Default\n        window.term.write('\u001B[Ea')\n        // Explicit\n        window.term.write('\u001B[2Eb')\n      ")];
                        case 1:
                            _c.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(4)];
                        case 2:
                            _b.apply(_a, [_c.sent(), ['', 'a', '', 'b']]);
                            return [2];
                    }
                });
            });
        });
        it('CPL: Cursor Preceding Line Ps Times (default = 1) - CSI Ps F', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0: return [4, page.evaluate("\n        // Default\n        window.term.write('\\n\\n\\n\\n\u001B[Fa')\n        // Explicit\n        window.term.write('\u001B[2Fb')\n      ")];
                        case 1:
                            _c.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(5)];
                        case 2:
                            _b.apply(_a, [_c.sent(), ['', 'b', '', 'a', '']]);
                            return [2];
                    }
                });
            });
        });
        it('CHA: Cursor Character Absolute [column] (default = [row,1]) - CSI Ps G', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0: return [4, page.evaluate("\n        // Default\n        window.term.write('foo\u001B[Ga')\n        // Explicit\n        window.term.write('\u001B[10Gb')\n      ")];
                        case 1:
                            _c.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(1)];
                        case 2:
                            _b.apply(_a, [_c.sent(), ['aoo      b']]);
                            return [2];
                    }
                });
            });
        });
        it('CUP: Cursor Position [row;column] (default = [1,1]) - CSI Ps ; Ps H', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0: return [4, page.evaluate("\n        // Default\n        window.term.write('foo\u001B[Ha')\n        // Explicit\n        window.term.write('\u001B[3;3Hb')\n      ")];
                        case 1:
                            _c.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(3)];
                        case 2:
                            _b.apply(_a, [_c.sent(), ['aoo', '', '  b']]);
                            return [2];
                    }
                });
            });
        });
        it('CHT: Cursor Forward Tabulation Ps tab stops (default = 1) - CSI Ps I', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0: return [4, page.evaluate("\n        // Default\n        window.term.write('\u001B[Ia')\n        // Explicit\n        window.term.write('\\n\\r\u001B[2Ib')\n      ")];
                        case 1:
                            _c.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(2)];
                        case 2:
                            _b.apply(_a, [_c.sent(), ['        a', '                b']]);
                            return [2];
                    }
                });
            });
        });
        it('ED: Erase in Display, VT100 - CSI Ps J', function () {
            return __awaiter(this, void 0, void 0, function () {
                var fixture, _a, _b, _c, _d, _e, _f, _g, _h, _j, _k;
                return __generator(this, function (_l) {
                    switch (_l.label) {
                        case 0:
                            fixture = 'abc\\n\\rdef\\n\\rghi\x1b[2;2H';
                            return [4, page.evaluate("\n        // Default: Erase Below\n        window.term.resize(5, 5);\n        window.term.write('" + fixture + "\u001B[J')\n      ")];
                        case 1:
                            _l.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(3)];
                        case 2:
                            _b.apply(_a, [_l.sent(), ['abc', 'd', '']]);
                            return [4, page.evaluate("\n        // 0: Erase Below\n        window.term.reset()\n        window.term.write('" + fixture + "\u001B[0J')\n      ")];
                        case 3:
                            _l.sent();
                            _d = (_c = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(3)];
                        case 4:
                            _d.apply(_c, [_l.sent(), ['abc', 'd', '']]);
                            return [4, page.evaluate("\n        // 1: Erase Above\n        window.term.reset()\n        window.term.write('" + fixture + "\u001B[1J')\n      ")];
                        case 5:
                            _l.sent();
                            _f = (_e = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(3)];
                        case 6:
                            _f.apply(_e, [_l.sent(), ['', '  f', 'ghi']]);
                            return [4, page.evaluate("\n        // 2: Erase Saved Lines (scrollback)\n        window.term.reset()\n        window.term.write('1\\n2\\n3\\n4\\n5" + fixture + "\u001B[3J')\n      ")];
                        case 7:
                            _l.sent();
                            _h = (_g = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.length")];
                        case 8:
                            _h.apply(_g, [_l.sent(), 5]);
                            _k = (_j = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(5)];
                        case 9:
                            _k.apply(_j, [_l.sent(), ['   4', '    5', 'abc', 'def', 'ghi']]);
                            return [2];
                    }
                });
            });
        });
        it('DECSED: Erase in Display, VT220 - CSI ? Ps J', function () {
            return __awaiter(this, void 0, void 0, function () {
                var fixture, _a, _b, _c, _d, _e, _f, _g, _h, _j, _k;
                return __generator(this, function (_l) {
                    switch (_l.label) {
                        case 0:
                            fixture = 'abc\\n\\rdef\\n\\rghi\x1b[2;2H';
                            return [4, page.evaluate("\n        // Default: Erase Below\n        window.term.resize(5, 5);\n        window.term.write('" + fixture + "\u001B[?J')\n      ")];
                        case 1:
                            _l.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(3)];
                        case 2:
                            _b.apply(_a, [_l.sent(), ['abc', 'd', '']]);
                            return [4, page.evaluate("\n        // 0: Erase Below\n        window.term.reset()\n        window.term.write('" + fixture + "\u001B[?0J')\n      ")];
                        case 3:
                            _l.sent();
                            _d = (_c = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(3)];
                        case 4:
                            _d.apply(_c, [_l.sent(), ['abc', 'd', '']]);
                            return [4, page.evaluate("\n        // 1: Erase Above\n        window.term.reset()\n        window.term.write('" + fixture + "\u001B[?1J')\n      ")];
                        case 5:
                            _l.sent();
                            _f = (_e = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(3)];
                        case 6:
                            _f.apply(_e, [_l.sent(), ['', '  f', 'ghi']]);
                            return [4, page.evaluate("\n        // 2: Erase Saved Lines (scrollback)\n        window.term.reset()\n        window.term.write('1\\n2\\n3\\n4\\n5" + fixture + "\u001B[?3J')\n      ")];
                        case 7:
                            _l.sent();
                            _h = (_g = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.length")];
                        case 8:
                            _h.apply(_g, [_l.sent(), 5]);
                            _k = (_j = chai_1.assert).deepEqual;
                            return [4, getLinesAsArray(5)];
                        case 9:
                            _k.apply(_j, [_l.sent(), ['   4', '    5', 'abc', 'def', 'ghi']]);
                            return [2];
                    }
                });
            });
        });
        describe('DSR: Device Status Report', function () {
            it('Status Report - CSI 5 n', function () {
                return __awaiter(this, void 0, void 0, function () {
                    var _a, _b;
                    return __generator(this, function (_c) {
                        switch (_c.label) {
                            case 0: return [4, page.evaluate("\n          window.term.onData(e => window.result = e);\n          window.term.write('\\x1b[5n');\n        ")];
                            case 1:
                                _c.sent();
                                _b = (_a = chai_1.assert).equal;
                                return [4, page.evaluate("window.result")];
                            case 2:
                                _b.apply(_a, [_c.sent(), '\x1b[0n']);
                                return [2];
                        }
                    });
                });
            });
            it('Report Cursor Position (CPR) - CSI 6 n', function () {
                return __awaiter(this, void 0, void 0, function () {
                    var _a, _b, _c, _d;
                    return __generator(this, function (_e) {
                        switch (_e.label) {
                            case 0: return [4, page.evaluate("window.term.write('\\n\\nfoo')")];
                            case 1:
                                _e.sent();
                                _b = (_a = chai_1.assert).deepEqual;
                                return [4, page.evaluate("\n          [window.term.buffer.cursorY, window.term.buffer.cursorX]\n        ")];
                            case 2:
                                _b.apply(_a, [_e.sent(), [2, 3]]);
                                return [4, page.evaluate("\n          window.term.onData(e => window.result = e);\n          window.term.write('\\x1b[6n');\n        ")];
                            case 3:
                                _e.sent();
                                _d = (_c = chai_1.assert).equal;
                                return [4, page.evaluate("window.result")];
                            case 4:
                                _d.apply(_c, [_e.sent(), '\x1b[3;4R']);
                                return [2];
                        }
                    });
                });
            });
            it('Report Cursor Position (DECXCPR) - CSI ? 6 n', function () {
                return __awaiter(this, void 0, void 0, function () {
                    var _a, _b, _c, _d;
                    return __generator(this, function (_e) {
                        switch (_e.label) {
                            case 0: return [4, page.evaluate("window.term.write('\\n\\nfoo')")];
                            case 1:
                                _e.sent();
                                _b = (_a = chai_1.assert).deepEqual;
                                return [4, page.evaluate("\n          [window.term.buffer.cursorY, window.term.buffer.cursorX]\n        ")];
                            case 2:
                                _b.apply(_a, [_e.sent(), [2, 3]]);
                                return [4, page.evaluate("\n          window.term.onData(e => window.result = e);\n          window.term.write('\\x1b[?6n');\n        ")];
                            case 3:
                                _e.sent();
                                _d = (_c = chai_1.assert).equal;
                                return [4, page.evaluate("window.result")];
                            case 4:
                                _d.apply(_c, [_e.sent(), '\x1b[?3;4R']);
                                return [2];
                        }
                    });
                });
            });
        });
    });
});
function openTerminal(options) {
    if (options === void 0) { options = {}; }
    return __awaiter(this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4, page.evaluate("window.term = new Terminal(" + JSON.stringify(options) + ")")];
                case 1:
                    _a.sent();
                    return [4, page.evaluate("window.term.open(document.querySelector('#terminal-container'))")];
                case 2:
                    _a.sent();
                    if (!(options.rendererType === 'dom')) return [3, 4];
                    return [4, page.waitForSelector('.xterm-rows')];
                case 3:
                    _a.sent();
                    return [3, 6];
                case 4: return [4, page.waitForSelector('.xterm-text-layer')];
                case 5:
                    _a.sent();
                    _a.label = 6;
                case 6: return [2];
            }
        });
    });
}
function getLinesAsArray(count, start) {
    if (start === void 0) { start = 0; }
    return __awaiter(this, void 0, void 0, function () {
        var text, i;
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0:
                    text = '';
                    for (i = start; i < start + count; i++) {
                        text += "window.term.buffer.getLine(" + i + ").translateToString(true),";
                    }
                    return [4, page.evaluate("[" + text + "]")];
                case 1: return [2, _a.sent()];
            }
        });
    });
}
//# sourceMappingURL=InputHandler.api.js.map