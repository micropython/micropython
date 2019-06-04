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
describe('API Integration Tests', function () {
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
                        return [2];
                }
            });
        });
    });
    after(function () {
        browser.close();
    });
    beforeEach(function () { return __awaiter(_this, void 0, void 0, function () {
        return __generator(this, function (_a) {
            switch (_a.label) {
                case 0: return [4, page.goto(APP)];
                case 1:
                    _a.sent();
                    return [2];
            }
        });
    }); });
    it('Default options', function () {
        return __awaiter(this, void 0, void 0, function () {
            var _a, _b, _c, _d;
            return __generator(this, function (_e) {
                switch (_e.label) {
                    case 0: return [4, openTerminal()];
                    case 1:
                        _e.sent();
                        _b = (_a = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.cols")];
                    case 2:
                        _b.apply(_a, [_e.sent(), 80]);
                        _d = (_c = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.rows")];
                    case 3:
                        _d.apply(_c, [_e.sent(), 24]);
                        return [2];
                }
            });
        });
    });
    it('write', function () {
        return __awaiter(this, void 0, void 0, function () {
            var _a, _b;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0: return [4, openTerminal()];
                    case 1:
                        _c.sent();
                        return [4, page.evaluate("\n      window.term.write('foo');\n      window.term.write('bar');\n      window.term.write('\u6587');\n    ")];
                    case 2:
                        _c.sent();
                        _b = (_a = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.buffer.getLine(0).translateToString(true)")];
                    case 3:
                        _b.apply(_a, [_c.sent(), 'foobar文']);
                        return [2];
                }
            });
        });
    });
    it('writeln', function () {
        return __awaiter(this, void 0, void 0, function () {
            var _a, _b, _c, _d, _e, _f;
            return __generator(this, function (_g) {
                switch (_g.label) {
                    case 0: return [4, openTerminal()];
                    case 1:
                        _g.sent();
                        return [4, page.evaluate("\n      window.term.writeln('foo');\n      window.term.writeln('bar');\n      window.term.writeln('\u6587');\n    ")];
                    case 2:
                        _g.sent();
                        _b = (_a = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.buffer.getLine(0).translateToString(true)")];
                    case 3:
                        _b.apply(_a, [_g.sent(), 'foo']);
                        _d = (_c = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.buffer.getLine(1).translateToString(true)")];
                    case 4:
                        _d.apply(_c, [_g.sent(), 'bar']);
                        _f = (_e = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.buffer.getLine(2).translateToString(true)")];
                    case 5:
                        _f.apply(_e, [_g.sent(), '文']);
                        return [2];
                }
            });
        });
    });
    it('writeUtf8', function () {
        return __awaiter(this, void 0, void 0, function () {
            var _a, _b;
            return __generator(this, function (_c) {
                switch (_c.label) {
                    case 0: return [4, openTerminal()];
                    case 1:
                        _c.sent();
                        return [4, page.evaluate("\n      // foo\n      window.term.writeUtf8(new Uint8Array([102, 111, 111]));\n      // bar\n      window.term.writeUtf8(new Uint8Array([98, 97, 114]));\n      // \u6587\n      window.term.writeUtf8(new Uint8Array([230, 150, 135]));\n    ")];
                    case 2:
                        _c.sent();
                        _b = (_a = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.buffer.getLine(0).translateToString(true)")];
                    case 3:
                        _b.apply(_a, [_c.sent(), 'foobar文']);
                        return [2];
                }
            });
        });
    });
    it('clear', function () {
        return __awaiter(this, void 0, void 0, function () {
            var _a, _b, _c, _d, i, _e, _f;
            return __generator(this, function (_g) {
                switch (_g.label) {
                    case 0: return [4, openTerminal({ rows: 5 })];
                    case 1:
                        _g.sent();
                        return [4, page.evaluate("\n      window.term.write('test0');\n      for (let i = 1; i < 10; i++) {\n        window.term.write('\\n\\rtest' + i);\n      }\n    ")];
                    case 2:
                        _g.sent();
                        return [4, page.evaluate("window.term.clear()")];
                    case 3:
                        _g.sent();
                        _b = (_a = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.buffer.length")];
                    case 4:
                        _b.apply(_a, [_g.sent(), '5']);
                        _d = (_c = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.buffer.getLine(0).translateToString(true)")];
                    case 5:
                        _d.apply(_c, [_g.sent(), 'test9']);
                        i = 1;
                        _g.label = 6;
                    case 6:
                        if (!(i < 5)) return [3, 9];
                        _f = (_e = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.buffer.getLine(" + i + ").translateToString(true)")];
                    case 7:
                        _f.apply(_e, [_g.sent(), '']);
                        _g.label = 8;
                    case 8:
                        i++;
                        return [3, 6];
                    case 9: return [2];
                }
            });
        });
    });
    it('getOption, setOption', function () {
        return __awaiter(this, void 0, void 0, function () {
            var _a, _b, _c, _d;
            return __generator(this, function (_e) {
                switch (_e.label) {
                    case 0: return [4, openTerminal()];
                    case 1:
                        _e.sent();
                        _b = (_a = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.getOption('rendererType')")];
                    case 2:
                        _b.apply(_a, [_e.sent(), 'canvas']);
                        return [4, page.evaluate("window.term.setOption('rendererType', 'dom')")];
                    case 3:
                        _e.sent();
                        _d = (_c = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.getOption('rendererType')")];
                    case 4:
                        _d.apply(_c, [_e.sent(), 'dom']);
                        return [2];
                }
            });
        });
    });
    describe('renderer', function () {
        it('foreground', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0: return [4, openTerminal({ rendererType: 'dom' })];
                        case 1:
                            _c.sent();
                            return [4, page.evaluate("window.term.write('\\x1b[30m0\\x1b[31m1\\x1b[32m2\\x1b[33m3\\x1b[34m4\\x1b[35m5\\x1b[36m6\\x1b[37m7')")];
                        case 2:
                            _c.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, page.evaluate("\n        [\n          document.querySelector('.xterm-rows > :nth-child(1) > :nth-child(1)').className,\n          document.querySelector('.xterm-rows > :nth-child(1) > :nth-child(2)').className,\n          document.querySelector('.xterm-rows > :nth-child(1) > :nth-child(3)').className,\n          document.querySelector('.xterm-rows > :nth-child(1) > :nth-child(4)').className,\n          document.querySelector('.xterm-rows > :nth-child(1) > :nth-child(5)').className,\n          document.querySelector('.xterm-rows > :nth-child(1) > :nth-child(6)').className,\n          document.querySelector('.xterm-rows > :nth-child(1) > :nth-child(7)').className\n        ]\n      ")];
                        case 3:
                            _b.apply(_a, [_c.sent(), [
                                    'xterm-fg-0',
                                    'xterm-fg-1',
                                    'xterm-fg-2',
                                    'xterm-fg-3',
                                    'xterm-fg-4',
                                    'xterm-fg-5',
                                    'xterm-fg-6'
                                ]]);
                            return [2];
                    }
                });
            });
        });
        it('background', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0: return [4, openTerminal({ rendererType: 'dom' })];
                        case 1:
                            _c.sent();
                            return [4, page.evaluate("window.term.write('\\x1b[40m0\\x1b[41m1\\x1b[42m2\\x1b[43m3\\x1b[44m4\\x1b[45m5\\x1b[46m6\\x1b[47m7')")];
                        case 2:
                            _c.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, page.evaluate("\n        [\n          document.querySelector('.xterm-rows > :nth-child(1) > :nth-child(1)').className,\n          document.querySelector('.xterm-rows > :nth-child(1) > :nth-child(2)').className,\n          document.querySelector('.xterm-rows > :nth-child(1) > :nth-child(3)').className,\n          document.querySelector('.xterm-rows > :nth-child(1) > :nth-child(4)').className,\n          document.querySelector('.xterm-rows > :nth-child(1) > :nth-child(5)').className,\n          document.querySelector('.xterm-rows > :nth-child(1) > :nth-child(6)').className,\n          document.querySelector('.xterm-rows > :nth-child(1) > :nth-child(7)').className\n        ]\n      ")];
                        case 3:
                            _b.apply(_a, [_c.sent(), [
                                    'xterm-bg-0',
                                    'xterm-bg-1',
                                    'xterm-bg-2',
                                    'xterm-bg-3',
                                    'xterm-bg-4',
                                    'xterm-bg-5',
                                    'xterm-bg-6'
                                ]]);
                            return [2];
                    }
                });
            });
        });
    });
    it('selection', function () {
        return __awaiter(this, void 0, void 0, function () {
            var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m, _o, _p, _q, _r, _s, _t, _u, _v, _w, _x, _y, _z;
            return __generator(this, function (_0) {
                switch (_0.label) {
                    case 0: return [4, openTerminal({ rows: 5, cols: 5 })];
                    case 1:
                        _0.sent();
                        return [4, page.evaluate("window.term.write('\\n\\nfoo\\n\\n\\rbar\\n\\n\\rbaz')")];
                    case 2:
                        _0.sent();
                        _b = (_a = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.hasSelection()")];
                    case 3:
                        _b.apply(_a, [_0.sent(), false]);
                        _d = (_c = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.getSelection()")];
                    case 4:
                        _d.apply(_c, [_0.sent(), '']);
                        _f = (_e = chai_1.assert).deepEqual;
                        return [4, page.evaluate("window.term.getSelectionPosition()")];
                    case 5:
                        _f.apply(_e, [_0.sent(), undefined]);
                        return [4, page.evaluate("window.term.selectAll()")];
                    case 6:
                        _0.sent();
                        _h = (_g = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.hasSelection()")];
                    case 7:
                        _h.apply(_g, [_0.sent(), true]);
                        _k = (_j = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.getSelection()")];
                    case 8:
                        _k.apply(_j, [_0.sent(), '\n\nfoo\n\nbar\n\nbaz']);
                        _m = (_l = chai_1.assert).deepEqual;
                        return [4, page.evaluate("window.term.getSelectionPosition()")];
                    case 9:
                        _m.apply(_l, [_0.sent(), { startColumn: 0, startRow: 0, endColumn: 5, endRow: 6 }]);
                        return [4, page.evaluate("window.term.clearSelection()")];
                    case 10:
                        _0.sent();
                        _p = (_o = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.hasSelection()")];
                    case 11:
                        _p.apply(_o, [_0.sent(), false]);
                        _r = (_q = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.getSelection()")];
                    case 12:
                        _r.apply(_q, [_0.sent(), '']);
                        _t = (_s = chai_1.assert).deepEqual;
                        return [4, page.evaluate("window.term.getSelectionPosition()")];
                    case 13:
                        _t.apply(_s, [_0.sent(), undefined]);
                        return [4, page.evaluate("window.term.select(1, 2, 2)")];
                    case 14:
                        _0.sent();
                        _v = (_u = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.hasSelection()")];
                    case 15:
                        _v.apply(_u, [_0.sent(), true]);
                        _x = (_w = chai_1.assert).equal;
                        return [4, page.evaluate("window.term.getSelection()")];
                    case 16:
                        _x.apply(_w, [_0.sent(), 'oo']);
                        _z = (_y = chai_1.assert).deepEqual;
                        return [4, page.evaluate("window.term.getSelectionPosition()")];
                    case 17:
                        _z.apply(_y, [_0.sent(), { startColumn: 1, startRow: 2, endColumn: 3, endRow: 2 }]);
                        return [2];
                }
            });
        });
    });
    it('focus, blur', function () {
        return __awaiter(this, void 0, void 0, function () {
            var _a, _b, _c, _d, _e, _f;
            return __generator(this, function (_g) {
                switch (_g.label) {
                    case 0: return [4, openTerminal()];
                    case 1:
                        _g.sent();
                        _b = (_a = chai_1.assert).equal;
                        return [4, page.evaluate("document.activeElement.className")];
                    case 2:
                        _b.apply(_a, [_g.sent(), '']);
                        return [4, page.evaluate("window.term.focus()")];
                    case 3:
                        _g.sent();
                        _d = (_c = chai_1.assert).equal;
                        return [4, page.evaluate("document.activeElement.className")];
                    case 4:
                        _d.apply(_c, [_g.sent(), 'xterm-helper-textarea']);
                        return [4, page.evaluate("window.term.blur()")];
                    case 5:
                        _g.sent();
                        _f = (_e = chai_1.assert).equal;
                        return [4, page.evaluate("document.activeElement.className")];
                    case 6:
                        _f.apply(_e, [_g.sent(), '']);
                        return [2];
                }
            });
        });
    });
    describe('loadAddon', function () {
        it('constructor', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0: return [4, openTerminal({ cols: 5 })];
                        case 1:
                            _c.sent();
                            return [4, page.evaluate("\n        window.cols = 0;\n        window.term.loadAddon({\n          activate: (t) => window.cols = t.cols,\n          dispose: () => {}\n        });\n      ")];
                        case 2:
                            _c.sent();
                            _b = (_a = chai_1.assert).equal;
                            return [4, page.evaluate("window.cols")];
                        case 3:
                            _b.apply(_a, [_c.sent(), 5]);
                            return [2];
                    }
                });
            });
        });
        it('dispose (addon)', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b, _c, _d;
                return __generator(this, function (_e) {
                    switch (_e.label) {
                        case 0: return [4, openTerminal()];
                        case 1:
                            _e.sent();
                            return [4, page.evaluate("\n        window.disposeCalled = false\n        window.addon = {\n          activate: () => {},\n          dispose: () => window.disposeCalled = true\n        };\n        window.term.loadAddon(window.addon);\n      ")];
                        case 2:
                            _e.sent();
                            _b = (_a = chai_1.assert).equal;
                            return [4, page.evaluate("window.disposeCalled")];
                        case 3:
                            _b.apply(_a, [_e.sent(), false]);
                            return [4, page.evaluate("window.addon.dispose()")];
                        case 4:
                            _e.sent();
                            _d = (_c = chai_1.assert).equal;
                            return [4, page.evaluate("window.disposeCalled")];
                        case 5:
                            _d.apply(_c, [_e.sent(), true]);
                            return [2];
                    }
                });
            });
        });
        it('dispose (terminal)', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b, _c, _d;
                return __generator(this, function (_e) {
                    switch (_e.label) {
                        case 0: return [4, openTerminal()];
                        case 1:
                            _e.sent();
                            return [4, page.evaluate("\n        window.disposeCalled = false\n        window.term.loadAddon({\n          activate: () => {},\n          dispose: () => window.disposeCalled = true\n        });\n      ")];
                        case 2:
                            _e.sent();
                            _b = (_a = chai_1.assert).equal;
                            return [4, page.evaluate("window.disposeCalled")];
                        case 3:
                            _b.apply(_a, [_e.sent(), false]);
                            return [4, page.evaluate("window.term.dispose()")];
                        case 4:
                            _e.sent();
                            _d = (_c = chai_1.assert).equal;
                            return [4, page.evaluate("window.disposeCalled")];
                        case 5:
                            _d.apply(_c, [_e.sent(), true]);
                            return [2];
                    }
                });
            });
        });
    });
    describe('Events', function () {
        it('onCursorMove', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b, _c, _d;
                return __generator(this, function (_e) {
                    switch (_e.label) {
                        case 0: return [4, openTerminal()];
                        case 1:
                            _e.sent();
                            return [4, page.evaluate("\n        window.callCount = 0;\n        window.term.onCursorMove(e => window.callCount++);\n        window.term.write('foo');\n      ")];
                        case 2:
                            _e.sent();
                            _b = (_a = chai_1.assert).equal;
                            return [4, page.evaluate("window.callCount")];
                        case 3:
                            _b.apply(_a, [_e.sent(), 1]);
                            return [4, page.evaluate("window.term.write('bar')")];
                        case 4:
                            _e.sent();
                            _d = (_c = chai_1.assert).equal;
                            return [4, page.evaluate("window.callCount")];
                        case 5:
                            _d.apply(_c, [_e.sent(), 2]);
                            return [2];
                    }
                });
            });
        });
        it('onData', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0: return [4, openTerminal()];
                        case 1:
                            _c.sent();
                            return [4, page.evaluate("\n        window.calls = [];\n        window.term.onData(e => calls.push(e));\n      ")];
                        case 2:
                            _c.sent();
                            return [4, page.type('.xterm-helper-textarea', 'foo')];
                        case 3:
                            _c.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, page.evaluate("window.calls")];
                        case 4:
                            _b.apply(_a, [_c.sent(), ['f', 'o', 'o']]);
                            return [2];
                    }
                });
            });
        });
        it('onKey', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b;
                return __generator(this, function (_c) {
                    switch (_c.label) {
                        case 0: return [4, openTerminal()];
                        case 1:
                            _c.sent();
                            return [4, page.evaluate("\n        window.calls = [];\n        window.term.onKey(e => calls.push(e.key));\n      ")];
                        case 2:
                            _c.sent();
                            return [4, page.type('.xterm-helper-textarea', 'foo')];
                        case 3:
                            _c.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, page.evaluate("window.calls")];
                        case 4:
                            _b.apply(_a, [_c.sent(), ['f', 'o', 'o']]);
                            return [2];
                    }
                });
            });
        });
        it('onLineFeed', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b, _c, _d;
                return __generator(this, function (_e) {
                    switch (_e.label) {
                        case 0: return [4, openTerminal()];
                        case 1:
                            _e.sent();
                            return [4, page.evaluate("\n        window.callCount = 0;\n        window.term.onLineFeed(() => callCount++);\n        window.term.writeln('foo');\n      ")];
                        case 2:
                            _e.sent();
                            _b = (_a = chai_1.assert).equal;
                            return [4, page.evaluate("window.callCount")];
                        case 3:
                            _b.apply(_a, [_e.sent(), 1]);
                            return [4, page.evaluate("window.term.writeln('bar')")];
                        case 4:
                            _e.sent();
                            _d = (_c = chai_1.assert).equal;
                            return [4, page.evaluate("window.callCount")];
                        case 5:
                            _d.apply(_c, [_e.sent(), 2]);
                            return [2];
                    }
                });
            });
        });
        it('onScroll', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b, _c, _d, _e, _f;
                return __generator(this, function (_g) {
                    switch (_g.label) {
                        case 0: return [4, openTerminal({ rows: 5 })];
                        case 1:
                            _g.sent();
                            return [4, page.evaluate("\n        window.calls = [];\n        window.term.onScroll(e => window.calls.push(e));\n        for (let i = 0; i < 4; i++) {\n          window.term.writeln('foo');\n        }\n      ")];
                        case 2:
                            _g.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, page.evaluate("window.calls")];
                        case 3:
                            _b.apply(_a, [_g.sent(), []]);
                            return [4, page.evaluate("window.term.writeln('bar')")];
                        case 4:
                            _g.sent();
                            _d = (_c = chai_1.assert).deepEqual;
                            return [4, page.evaluate("window.calls")];
                        case 5:
                            _d.apply(_c, [_g.sent(), [1]]);
                            return [4, page.evaluate("window.term.writeln('baz')")];
                        case 6:
                            _g.sent();
                            _f = (_e = chai_1.assert).deepEqual;
                            return [4, page.evaluate("window.calls")];
                        case 7:
                            _f.apply(_e, [_g.sent(), [1, 2]]);
                            return [2];
                    }
                });
            });
        });
        it('onSelectionChange', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b, _c, _d, _e, _f;
                return __generator(this, function (_g) {
                    switch (_g.label) {
                        case 0: return [4, openTerminal()];
                        case 1:
                            _g.sent();
                            return [4, page.evaluate("\n        window.callCount = 0;\n        window.term.onSelectionChange(() => window.callCount++);\n      ")];
                        case 2:
                            _g.sent();
                            _b = (_a = chai_1.assert).equal;
                            return [4, page.evaluate("window.callCount")];
                        case 3:
                            _b.apply(_a, [_g.sent(), 0]);
                            return [4, page.evaluate("window.term.selectAll()")];
                        case 4:
                            _g.sent();
                            _d = (_c = chai_1.assert).equal;
                            return [4, page.evaluate("window.callCount")];
                        case 5:
                            _d.apply(_c, [_g.sent(), 1]);
                            return [4, page.evaluate("window.term.clearSelection()")];
                        case 6:
                            _g.sent();
                            _f = (_e = chai_1.assert).equal;
                            return [4, page.evaluate("window.callCount")];
                        case 7:
                            _f.apply(_e, [_g.sent(), 2]);
                            return [2];
                    }
                });
            });
        });
        it('onRender', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b, _c, _d, _e, _f;
                return __generator(this, function (_g) {
                    switch (_g.label) {
                        case 0: return [4, openTerminal()];
                        case 1:
                            _g.sent();
                            return [4, page.evaluate("\n        window.calls = [];\n        window.term.onRender(e => window.calls.push([e.start, e.end]));\n      ")];
                        case 2:
                            _g.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, page.evaluate("window.calls")];
                        case 3:
                            _b.apply(_a, [_g.sent(), []]);
                            return [4, page.evaluate("window.term.write('foo')")];
                        case 4:
                            _g.sent();
                            _d = (_c = chai_1.assert).deepEqual;
                            return [4, page.evaluate("window.calls")];
                        case 5:
                            _d.apply(_c, [_g.sent(), [[0, 0]]]);
                            return [4, page.evaluate("window.term.write('bar\\n\\nbaz')")];
                        case 6:
                            _g.sent();
                            _f = (_e = chai_1.assert).deepEqual;
                            return [4, page.evaluate("window.calls")];
                        case 7:
                            _f.apply(_e, [_g.sent(), [[0, 0], [0, 2]]]);
                            return [2];
                    }
                });
            });
        });
        it('onResize', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b, _c, _d, _e, _f;
                return __generator(this, function (_g) {
                    switch (_g.label) {
                        case 0: return [4, openTerminal()];
                        case 1:
                            _g.sent();
                            return [4, page.evaluate("\n        window.calls = [];\n        window.term.onResize(e => window.calls.push([e.cols, e.rows]));\n      ")];
                        case 2:
                            _g.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, page.evaluate("window.calls")];
                        case 3:
                            _b.apply(_a, [_g.sent(), []]);
                            return [4, page.evaluate("window.term.resize(10, 5)")];
                        case 4:
                            _g.sent();
                            _d = (_c = chai_1.assert).deepEqual;
                            return [4, page.evaluate("window.calls")];
                        case 5:
                            _d.apply(_c, [_g.sent(), [[10, 5]]]);
                            return [4, page.evaluate("window.term.resize(20, 15)")];
                        case 6:
                            _g.sent();
                            _f = (_e = chai_1.assert).deepEqual;
                            return [4, page.evaluate("window.calls")];
                        case 7:
                            _f.apply(_e, [_g.sent(), [[10, 5], [20, 15]]]);
                            return [2];
                    }
                });
            });
        });
        it('onTitleChange', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b, _c, _d;
                return __generator(this, function (_e) {
                    switch (_e.label) {
                        case 0: return [4, openTerminal()];
                        case 1:
                            _e.sent();
                            return [4, page.evaluate("\n        window.calls = [];\n        window.term.onTitleChange(e => window.calls.push(e));\n      ")];
                        case 2:
                            _e.sent();
                            _b = (_a = chai_1.assert).deepEqual;
                            return [4, page.evaluate("window.calls")];
                        case 3:
                            _b.apply(_a, [_e.sent(), []]);
                            return [4, page.evaluate("window.term.write('\\x1b]2;foo\\x9c')")];
                        case 4:
                            _e.sent();
                            _d = (_c = chai_1.assert).deepEqual;
                            return [4, page.evaluate("window.calls")];
                        case 5:
                            _d.apply(_c, [_e.sent(), ['foo']]);
                            return [2];
                    }
                });
            });
        });
    });
    describe('buffer', function () {
        it('cursorX, cursorY', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m, _o, _p, _q, _r, _s, _t, _u, _v, _w, _x, _y, _z;
                return __generator(this, function (_0) {
                    switch (_0.label) {
                        case 0: return [4, openTerminal({ rows: 5, cols: 5 })];
                        case 1:
                            _0.sent();
                            _b = (_a = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.cursorX")];
                        case 2:
                            _b.apply(_a, [_0.sent(), 0]);
                            _d = (_c = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.cursorY")];
                        case 3:
                            _d.apply(_c, [_0.sent(), 0]);
                            return [4, page.evaluate("window.term.write('foo')")];
                        case 4:
                            _0.sent();
                            _f = (_e = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.cursorX")];
                        case 5:
                            _f.apply(_e, [_0.sent(), 3]);
                            _h = (_g = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.cursorY")];
                        case 6:
                            _h.apply(_g, [_0.sent(), 0]);
                            return [4, page.evaluate("window.term.write('\\n')")];
                        case 7:
                            _0.sent();
                            _k = (_j = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.cursorX")];
                        case 8:
                            _k.apply(_j, [_0.sent(), 3]);
                            _m = (_l = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.cursorY")];
                        case 9:
                            _m.apply(_l, [_0.sent(), 1]);
                            return [4, page.evaluate("window.term.write('\\r')")];
                        case 10:
                            _0.sent();
                            _p = (_o = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.cursorX")];
                        case 11:
                            _p.apply(_o, [_0.sent(), 0]);
                            _r = (_q = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.cursorY")];
                        case 12:
                            _r.apply(_q, [_0.sent(), 1]);
                            return [4, page.evaluate("window.term.write('abcde')")];
                        case 13:
                            _0.sent();
                            _t = (_s = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.cursorX")];
                        case 14:
                            _t.apply(_s, [_0.sent(), 5]);
                            _v = (_u = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.cursorY")];
                        case 15:
                            _v.apply(_u, [_0.sent(), 1]);
                            return [4, page.evaluate("window.term.write('\\n\\r\\n\\n\\n\\n\\n')")];
                        case 16:
                            _0.sent();
                            _x = (_w = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.cursorX")];
                        case 17:
                            _x.apply(_w, [_0.sent(), 0]);
                            _z = (_y = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.cursorY")];
                        case 18:
                            _z.apply(_y, [_0.sent(), 4]);
                            return [2];
                    }
                });
            });
        });
        it('viewportY', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m;
                return __generator(this, function (_o) {
                    switch (_o.label) {
                        case 0: return [4, openTerminal({ rows: 5 })];
                        case 1:
                            _o.sent();
                            _b = (_a = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.viewportY")];
                        case 2:
                            _b.apply(_a, [_o.sent(), 0]);
                            return [4, page.evaluate("window.term.write('\\n\\n\\n\\n')")];
                        case 3:
                            _o.sent();
                            _d = (_c = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.viewportY")];
                        case 4:
                            _d.apply(_c, [_o.sent(), 0]);
                            return [4, page.evaluate("window.term.write('\\n')")];
                        case 5:
                            _o.sent();
                            _f = (_e = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.viewportY")];
                        case 6:
                            _f.apply(_e, [_o.sent(), 1]);
                            return [4, page.evaluate("window.term.write('\\n\\n\\n\\n')")];
                        case 7:
                            _o.sent();
                            _h = (_g = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.viewportY")];
                        case 8:
                            _h.apply(_g, [_o.sent(), 5]);
                            return [4, page.evaluate("window.term.scrollLines(-1)")];
                        case 9:
                            _o.sent();
                            _k = (_j = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.viewportY")];
                        case 10:
                            _k.apply(_j, [_o.sent(), 4]);
                            return [4, page.evaluate("window.term.scrollToTop()")];
                        case 11:
                            _o.sent();
                            _m = (_l = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.viewportY")];
                        case 12:
                            _m.apply(_l, [_o.sent(), 0]);
                            return [2];
                    }
                });
            });
        });
        it('baseY', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m;
                return __generator(this, function (_o) {
                    switch (_o.label) {
                        case 0: return [4, openTerminal({ rows: 5 })];
                        case 1:
                            _o.sent();
                            _b = (_a = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.baseY")];
                        case 2:
                            _b.apply(_a, [_o.sent(), 0]);
                            return [4, page.evaluate("window.term.write('\\n\\n\\n\\n')")];
                        case 3:
                            _o.sent();
                            _d = (_c = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.baseY")];
                        case 4:
                            _d.apply(_c, [_o.sent(), 0]);
                            return [4, page.evaluate("window.term.write('\\n')")];
                        case 5:
                            _o.sent();
                            _f = (_e = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.baseY")];
                        case 6:
                            _f.apply(_e, [_o.sent(), 1]);
                            return [4, page.evaluate("window.term.write('\\n\\n\\n\\n')")];
                        case 7:
                            _o.sent();
                            _h = (_g = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.baseY")];
                        case 8:
                            _h.apply(_g, [_o.sent(), 5]);
                            return [4, page.evaluate("window.term.scrollLines(-1)")];
                        case 9:
                            _o.sent();
                            _k = (_j = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.baseY")];
                        case 10:
                            _k.apply(_j, [_o.sent(), 5]);
                            return [4, page.evaluate("window.term.scrollToTop()")];
                        case 11:
                            _o.sent();
                            _m = (_l = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.baseY")];
                        case 12:
                            _m.apply(_l, [_o.sent(), 5]);
                            return [2];
                    }
                });
            });
        });
        it('length', function () {
            return __awaiter(this, void 0, void 0, function () {
                var _a, _b, _c, _d, _e, _f, _g, _h;
                return __generator(this, function (_j) {
                    switch (_j.label) {
                        case 0: return [4, openTerminal({ rows: 5 })];
                        case 1:
                            _j.sent();
                            _b = (_a = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.length")];
                        case 2:
                            _b.apply(_a, [_j.sent(), 5]);
                            return [4, page.evaluate("window.term.write('\\n\\n\\n\\n')")];
                        case 3:
                            _j.sent();
                            _d = (_c = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.length")];
                        case 4:
                            _d.apply(_c, [_j.sent(), 5]);
                            return [4, page.evaluate("window.term.write('\\n')")];
                        case 5:
                            _j.sent();
                            _f = (_e = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.length")];
                        case 6:
                            _f.apply(_e, [_j.sent(), 6]);
                            return [4, page.evaluate("window.term.write('\\n\\n\\n\\n')")];
                        case 7:
                            _j.sent();
                            _h = (_g = chai_1.assert).equal;
                            return [4, page.evaluate("window.term.buffer.length")];
                        case 8:
                            _h.apply(_g, [_j.sent(), 10]);
                            return [2];
                    }
                });
            });
        });
        describe('getLine', function () {
            it('invalid index', function () {
                return __awaiter(this, void 0, void 0, function () {
                    var _a, _b, _c, _d;
                    return __generator(this, function (_e) {
                        switch (_e.label) {
                            case 0: return [4, openTerminal({ rows: 5 })];
                            case 1:
                                _e.sent();
                                _b = (_a = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(-1)")];
                            case 2:
                                _b.apply(_a, [_e.sent(), undefined]);
                                _d = (_c = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(5)")];
                            case 3:
                                _d.apply(_c, [_e.sent(), undefined]);
                                return [2];
                        }
                    });
                });
            });
            it('isWrapped', function () {
                return __awaiter(this, void 0, void 0, function () {
                    var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m;
                    return __generator(this, function (_o) {
                        switch (_o.label) {
                            case 0: return [4, openTerminal({ cols: 5 })];
                            case 1:
                                _o.sent();
                                _b = (_a = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).isWrapped")];
                            case 2:
                                _b.apply(_a, [_o.sent(), false]);
                                _d = (_c = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(1).isWrapped")];
                            case 3:
                                _d.apply(_c, [_o.sent(), false]);
                                return [4, page.evaluate("window.term.write('abcde')")];
                            case 4:
                                _o.sent();
                                _f = (_e = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).isWrapped")];
                            case 5:
                                _f.apply(_e, [_o.sent(), false]);
                                _h = (_g = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(1).isWrapped")];
                            case 6:
                                _h.apply(_g, [_o.sent(), false]);
                                return [4, page.evaluate("window.term.write('f')")];
                            case 7:
                                _o.sent();
                                _k = (_j = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).isWrapped")];
                            case 8:
                                _k.apply(_j, [_o.sent(), false]);
                                _m = (_l = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(1).isWrapped")];
                            case 9:
                                _m.apply(_l, [_o.sent(), true]);
                                return [2];
                        }
                    });
                });
            });
            it('translateToString', function () {
                return __awaiter(this, void 0, void 0, function () {
                    var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m, _o, _p, _q, _r, _s, _t;
                    return __generator(this, function (_u) {
                        switch (_u.label) {
                            case 0: return [4, openTerminal({ cols: 5 })];
                            case 1:
                                _u.sent();
                                _b = (_a = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).translateToString()")];
                            case 2:
                                _b.apply(_a, [_u.sent(), '     ']);
                                _d = (_c = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).translateToString(true)")];
                            case 3:
                                _d.apply(_c, [_u.sent(), '']);
                                return [4, page.evaluate("window.term.write('foo')")];
                            case 4:
                                _u.sent();
                                _f = (_e = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).translateToString()")];
                            case 5:
                                _f.apply(_e, [_u.sent(), 'foo  ']);
                                _h = (_g = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).translateToString(true)")];
                            case 6:
                                _h.apply(_g, [_u.sent(), 'foo']);
                                return [4, page.evaluate("window.term.write('bar')")];
                            case 7:
                                _u.sent();
                                _k = (_j = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).translateToString()")];
                            case 8:
                                _k.apply(_j, [_u.sent(), 'fooba']);
                                _m = (_l = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).translateToString(true)")];
                            case 9:
                                _m.apply(_l, [_u.sent(), 'fooba']);
                                _p = (_o = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(1).translateToString(true)")];
                            case 10:
                                _p.apply(_o, [_u.sent(), 'r']);
                                _r = (_q = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).translateToString(false, 1)")];
                            case 11:
                                _r.apply(_q, [_u.sent(), 'ooba']);
                                _t = (_s = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).translateToString(false, 1, 3)")];
                            case 12:
                                _t.apply(_s, [_u.sent(), 'oo']);
                                return [2];
                        }
                    });
                });
            });
            it('getCell', function () {
                return __awaiter(this, void 0, void 0, function () {
                    var _a, _b, _c, _d, _e, _f, _g, _h, _j, _k, _l, _m, _o, _p, _q, _r, _s, _t, _u, _v;
                    return __generator(this, function (_w) {
                        switch (_w.label) {
                            case 0: return [4, openTerminal({ cols: 5 })];
                            case 1:
                                _w.sent();
                                _b = (_a = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).getCell(-1)")];
                            case 2:
                                _b.apply(_a, [_w.sent(), undefined]);
                                _d = (_c = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).getCell(5)")];
                            case 3:
                                _d.apply(_c, [_w.sent(), undefined]);
                                _f = (_e = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).getCell(0).char")];
                            case 4:
                                _f.apply(_e, [_w.sent(), '']);
                                _h = (_g = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).getCell(0).width")];
                            case 5:
                                _h.apply(_g, [_w.sent(), 1]);
                                return [4, page.evaluate("window.term.write('a\u6587')")];
                            case 6:
                                _w.sent();
                                _k = (_j = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).getCell(0).char")];
                            case 7:
                                _k.apply(_j, [_w.sent(), 'a']);
                                _m = (_l = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).getCell(0).width")];
                            case 8:
                                _m.apply(_l, [_w.sent(), 1]);
                                _p = (_o = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).getCell(1).char")];
                            case 9:
                                _p.apply(_o, [_w.sent(), '文']);
                                _r = (_q = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).getCell(1).width")];
                            case 10:
                                _r.apply(_q, [_w.sent(), 2]);
                                _t = (_s = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).getCell(2).char")];
                            case 11:
                                _t.apply(_s, [_w.sent(), '']);
                                _v = (_u = chai_1.assert).equal;
                                return [4, page.evaluate("window.term.buffer.getLine(0).getCell(2).width")];
                            case 12:
                                _v.apply(_u, [_w.sent(), 0]);
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
//# sourceMappingURL=Terminal.api.js.map