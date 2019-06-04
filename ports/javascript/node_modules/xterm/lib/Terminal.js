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
var BufferSet_1 = require("./BufferSet");
var Buffer_1 = require("./Buffer");
var CompositionHelper_1 = require("./CompositionHelper");
var EventEmitter_1 = require("./common/EventEmitter");
var Viewport_1 = require("./Viewport");
var Clipboard_1 = require("./Clipboard");
var EscapeSequences_1 = require("./common/data/EscapeSequences");
var InputHandler_1 = require("./InputHandler");
var Renderer_1 = require("./renderer/Renderer");
var Linkifier_1 = require("./Linkifier");
var SelectionManager_1 = require("./SelectionManager");
var CharMeasure_1 = require("./CharMeasure");
var Browser = require("./common/Platform");
var Lifecycle_1 = require("./ui/Lifecycle");
var Strings = require("./Strings");
var MouseHelper_1 = require("./MouseHelper");
var SoundManager_1 = require("./SoundManager");
var MouseZoneManager_1 = require("./MouseZoneManager");
var AccessibilityManager_1 = require("./AccessibilityManager");
var CharAtlasCache_1 = require("./renderer/atlas/CharAtlasCache");
var DomRenderer_1 = require("./renderer/dom/DomRenderer");
var Keyboard_1 = require("./core/input/Keyboard");
var Clone_1 = require("./common/Clone");
var EventEmitter2_1 = require("./common/EventEmitter2");
var BufferLine_1 = require("./core/buffer/BufferLine");
var WindowsMode_1 = require("./WindowsMode");
var ColorManager_1 = require("./ui/ColorManager");
var RenderCoordinator_1 = require("./renderer/RenderCoordinator");
var document = (typeof window !== 'undefined') ? window.document : null;
var WRITE_BUFFER_PAUSE_THRESHOLD = 5;
var WRITE_TIMEOUT_MS = 12;
var WRITE_BUFFER_LENGTH_THRESHOLD = 50;
var MINIMUM_COLS = 2;
var MINIMUM_ROWS = 1;
var CONSTRUCTOR_ONLY_OPTIONS = ['cols', 'rows'];
var DEFAULT_OPTIONS = {
    cols: 80,
    rows: 24,
    convertEol: false,
    termName: 'xterm',
    cursorBlink: false,
    cursorStyle: 'block',
    bellSound: SoundManager_1.DEFAULT_BELL_SOUND,
    bellStyle: 'none',
    drawBoldTextInBrightColors: true,
    enableBold: true,
    experimentalCharAtlas: 'static',
    fontFamily: 'courier-new, courier, monospace',
    fontSize: 15,
    fontWeight: 'normal',
    fontWeightBold: 'bold',
    lineHeight: 1.0,
    letterSpacing: 0,
    scrollback: 1000,
    screenKeys: false,
    screenReaderMode: false,
    debug: false,
    macOptionIsMeta: false,
    macOptionClickForcesSelection: false,
    cancelEvents: false,
    disableStdin: false,
    useFlowControl: false,
    allowTransparency: false,
    tabStopWidth: 8,
    theme: undefined,
    rightClickSelectsWord: Browser.isMac,
    rendererType: 'canvas',
    windowsMode: false
};
var Terminal = (function (_super) {
    __extends(Terminal, _super);
    function Terminal(options) {
        if (options === void 0) { options = {}; }
        var _this = _super.call(this) || this;
        _this.browser = Browser;
        _this._blankLine = null;
        _this._onCursorMove = new EventEmitter2_1.EventEmitter2();
        _this._onData = new EventEmitter2_1.EventEmitter2();
        _this._onKey = new EventEmitter2_1.EventEmitter2();
        _this._onLineFeed = new EventEmitter2_1.EventEmitter2();
        _this._onRender = new EventEmitter2_1.EventEmitter2();
        _this._onResize = new EventEmitter2_1.EventEmitter2();
        _this._onScroll = new EventEmitter2_1.EventEmitter2();
        _this._onSelectionChange = new EventEmitter2_1.EventEmitter2();
        _this._onTitleChange = new EventEmitter2_1.EventEmitter2();
        _this.options = Clone_1.clone(options);
        _this._setup();
        _this.onCursorMove(function () { return _this.emit('cursormove'); });
        _this.onData(function (e) { return _this.emit('data', e); });
        _this.onKey(function (e) { return _this.emit('key', e.key, e.domEvent); });
        _this.onLineFeed(function () { return _this.emit('linefeed'); });
        _this.onRender(function (e) { return _this.emit('refresh', e); });
        _this.onResize(function (e) { return _this.emit('resize', e); });
        _this.onSelectionChange(function () { return _this.emit('selection'); });
        _this.onScroll(function (e) { return _this.emit('scroll', e); });
        _this.onTitleChange(function (e) { return _this.emit('title', e); });
        return _this;
    }
    Object.defineProperty(Terminal.prototype, "onCursorMove", {
        get: function () { return this._onCursorMove.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onData", {
        get: function () { return this._onData.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onKey", {
        get: function () { return this._onKey.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onLineFeed", {
        get: function () { return this._onLineFeed.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onRender", {
        get: function () { return this._onRender.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onResize", {
        get: function () { return this._onResize.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onScroll", {
        get: function () { return this._onScroll.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onSelectionChange", {
        get: function () { return this._onSelectionChange.event; },
        enumerable: true,
        configurable: true
    });
    Object.defineProperty(Terminal.prototype, "onTitleChange", {
        get: function () { return this._onTitleChange.event; },
        enumerable: true,
        configurable: true
    });
    Terminal.prototype.dispose = function () {
        _super.prototype.dispose.call(this);
        if (this._windowsMode) {
            this._windowsMode.dispose();
            this._windowsMode = undefined;
        }
        this._customKeyEventHandler = null;
        CharAtlasCache_1.removeTerminalFromCache(this);
        this.handler = function () { };
        this.write = function () { };
        if (this.element && this.element.parentNode) {
            this.element.parentNode.removeChild(this.element);
        }
    };
    Terminal.prototype.destroy = function () {
        this.dispose();
    };
    Terminal.prototype._setup = function () {
        var _this = this;
        Object.keys(DEFAULT_OPTIONS).forEach(function (key) {
            if (_this.options[key] === null || _this.options[key] === undefined) {
                _this.options[key] = DEFAULT_OPTIONS[key];
            }
        });
        this._parent = document ? document.body : null;
        this.cols = Math.max(this.options.cols, MINIMUM_COLS);
        this.rows = Math.max(this.options.rows, MINIMUM_ROWS);
        if (this.options.handler) {
            this.onData(this.options.handler);
        }
        this.cursorState = 0;
        this.cursorHidden = false;
        this._customKeyEventHandler = null;
        this.applicationKeypad = false;
        this.applicationCursor = false;
        this.originMode = false;
        this.insertMode = false;
        this.wraparoundMode = true;
        this.bracketedPasteMode = false;
        this.charset = null;
        this.gcharset = null;
        this.glevel = 0;
        this.charsets = [null];
        this.curAttrData = BufferLine_1.DEFAULT_ATTR_DATA.clone();
        this._eraseAttrData = BufferLine_1.DEFAULT_ATTR_DATA.clone();
        this.params = [];
        this.currentParam = 0;
        this.writeBuffer = [];
        this.writeBufferUtf8 = [];
        this._writeInProgress = false;
        this._xoffSentToCatchUp = false;
        this._userScrolling = false;
        this._inputHandler = new InputHandler_1.InputHandler(this);
        this._inputHandler.onCursorMove(function () { return _this._onCursorMove.fire(); });
        this._inputHandler.onLineFeed(function () { return _this._onLineFeed.fire(); });
        this._inputHandler.onData(function (e) { return _this._onData.fire(e); });
        this.register(this._inputHandler);
        this.selectionManager = this.selectionManager || null;
        this.linkifier = this.linkifier || new Linkifier_1.Linkifier(this);
        this._mouseZoneManager = this._mouseZoneManager || null;
        this.soundManager = this.soundManager || new SoundManager_1.SoundManager(this);
        this.buffers = new BufferSet_1.BufferSet(this);
        if (this.selectionManager) {
            this.selectionManager.clearSelection();
            this.selectionManager.initBuffersListeners();
        }
        if (this.options.windowsMode) {
            this._windowsMode = WindowsMode_1.applyWindowsMode(this);
        }
    };
    Object.defineProperty(Terminal.prototype, "buffer", {
        get: function () {
            return this.buffers.active;
        },
        enumerable: true,
        configurable: true
    });
    Terminal.prototype.eraseAttrData = function () {
        this._eraseAttrData.bg &= ~(50331648 | 0xFFFFFF);
        this._eraseAttrData.bg |= this.curAttrData.bg & ~0xFC000000;
        return this._eraseAttrData;
    };
    Terminal.prototype.focus = function () {
        if (this.textarea) {
            this.textarea.focus({ preventScroll: true });
        }
    };
    Object.defineProperty(Terminal.prototype, "isFocused", {
        get: function () {
            return document.activeElement === this.textarea && document.hasFocus();
        },
        enumerable: true,
        configurable: true
    });
    Terminal.prototype.getOption = function (key) {
        if (!(key in DEFAULT_OPTIONS)) {
            throw new Error('No option with key "' + key + '"');
        }
        return this.options[key];
    };
    Terminal.prototype.setOption = function (key, value) {
        if (!(key in DEFAULT_OPTIONS)) {
            throw new Error('No option with key "' + key + '"');
        }
        if (CONSTRUCTOR_ONLY_OPTIONS.indexOf(key) !== -1) {
            console.error("Option \"" + key + "\" can only be set in the constructor");
        }
        if (this.options[key] === value) {
            return;
        }
        switch (key) {
            case 'bellStyle':
                if (!value) {
                    value = 'none';
                }
                break;
            case 'cursorStyle':
                if (!value) {
                    value = 'block';
                }
                break;
            case 'fontWeight':
                if (!value) {
                    value = 'normal';
                }
                break;
            case 'fontWeightBold':
                if (!value) {
                    value = 'bold';
                }
                break;
            case 'lineHeight':
                if (value < 1) {
                    console.warn(key + " cannot be less than 1, value: " + value);
                    return;
                }
            case 'rendererType':
                if (!value) {
                    value = 'canvas';
                }
                break;
            case 'tabStopWidth':
                if (value < 1) {
                    console.warn(key + " cannot be less than 1, value: " + value);
                    return;
                }
                break;
            case 'theme':
                this._setTheme(value);
                break;
            case 'scrollback':
                value = Math.min(value, Buffer_1.MAX_BUFFER_SIZE);
                if (value < 0) {
                    console.warn(key + " cannot be less than 0, value: " + value);
                    return;
                }
                if (this.options[key] !== value) {
                    var newBufferLength = this.rows + value;
                    if (this.buffer.lines.length > newBufferLength) {
                        var amountToTrim = this.buffer.lines.length - newBufferLength;
                        var needsRefresh = (this.buffer.ydisp - amountToTrim < 0);
                        this.buffer.lines.trimStart(amountToTrim);
                        this.buffer.ybase = Math.max(this.buffer.ybase - amountToTrim, 0);
                        this.buffer.ydisp = Math.max(this.buffer.ydisp - amountToTrim, 0);
                        if (needsRefresh) {
                            this.refresh(0, this.rows - 1);
                        }
                    }
                }
                break;
        }
        this.options[key] = value;
        switch (key) {
            case 'fontFamily':
            case 'fontSize':
                if (this._renderCoordinator) {
                    this._renderCoordinator.clear();
                    this.charMeasure.measure(this.options);
                }
                break;
            case 'drawBoldTextInBrightColors':
            case 'experimentalCharAtlas':
            case 'enableBold':
            case 'letterSpacing':
            case 'lineHeight':
            case 'fontWeight':
            case 'fontWeightBold':
                if (this._renderCoordinator) {
                    this._renderCoordinator.clear();
                    this._renderCoordinator.onResize(this.cols, this.rows);
                    this.refresh(0, this.rows - 1);
                }
                break;
            case 'rendererType':
                if (this._renderCoordinator) {
                    this._renderCoordinator.setRenderer(this._createRenderer());
                }
                break;
            case 'scrollback':
                this.buffers.resize(this.cols, this.rows);
                if (this.viewport) {
                    this.viewport.syncScrollArea();
                }
                break;
            case 'screenReaderMode':
                if (value) {
                    if (!this._accessibilityManager && this._renderCoordinator) {
                        this._accessibilityManager = new AccessibilityManager_1.AccessibilityManager(this, this._renderCoordinator.dimensions);
                    }
                }
                else {
                    if (this._accessibilityManager) {
                        this._accessibilityManager.dispose();
                        this._accessibilityManager = null;
                    }
                }
                break;
            case 'tabStopWidth':
                this.buffers.setupTabStops();
                break;
            case 'windowsMode':
                if (value) {
                    if (!this._windowsMode) {
                        this._windowsMode = WindowsMode_1.applyWindowsMode(this);
                    }
                }
                else {
                    if (this._windowsMode) {
                        this._windowsMode.dispose();
                        this._windowsMode = undefined;
                    }
                }
                break;
        }
        if (this._renderCoordinator) {
            this._renderCoordinator.onOptionsChanged();
        }
    };
    Terminal.prototype._onTextAreaFocus = function (ev) {
        if (this.sendFocus) {
            this.handler(EscapeSequences_1.C0.ESC + '[I');
        }
        this.updateCursorStyle(ev);
        this.element.classList.add('focus');
        this.showCursor();
        this.emit('focus');
    };
    Terminal.prototype.blur = function () {
        return this.textarea.blur();
    };
    Terminal.prototype._onTextAreaBlur = function () {
        this.textarea.value = '';
        this.refresh(this.buffer.y, this.buffer.y);
        if (this.sendFocus) {
            this.handler(EscapeSequences_1.C0.ESC + '[O');
        }
        this.element.classList.remove('focus');
        this.emit('blur');
    };
    Terminal.prototype._initGlobal = function () {
        var _this = this;
        this._bindKeys();
        this.register(Lifecycle_1.addDisposableDomListener(this.element, 'copy', function (event) {
            if (!_this.hasSelection()) {
                return;
            }
            Clipboard_1.copyHandler(event, _this, _this.selectionManager);
        }));
        var pasteHandlerWrapper = function (event) { return Clipboard_1.pasteHandler(event, _this); };
        this.register(Lifecycle_1.addDisposableDomListener(this.textarea, 'paste', pasteHandlerWrapper));
        this.register(Lifecycle_1.addDisposableDomListener(this.element, 'paste', pasteHandlerWrapper));
        if (Browser.isFirefox) {
            this.register(Lifecycle_1.addDisposableDomListener(this.element, 'mousedown', function (event) {
                if (event.button === 2) {
                    Clipboard_1.rightClickHandler(event, _this, _this.selectionManager, _this.options.rightClickSelectsWord);
                }
            }));
        }
        else {
            this.register(Lifecycle_1.addDisposableDomListener(this.element, 'contextmenu', function (event) {
                Clipboard_1.rightClickHandler(event, _this, _this.selectionManager, _this.options.rightClickSelectsWord);
            }));
        }
        if (Browser.isLinux) {
            this.register(Lifecycle_1.addDisposableDomListener(this.element, 'auxclick', function (event) {
                if (event.button === 1) {
                    Clipboard_1.moveTextAreaUnderMouseCursor(event, _this);
                }
            }));
        }
    };
    Terminal.prototype._bindKeys = function () {
        var _this = this;
        var self = this;
        this.register(Lifecycle_1.addDisposableDomListener(this.element, 'keydown', function (ev) {
            if (document.activeElement !== this) {
                return;
            }
            self._keyDown(ev);
        }, true));
        this.register(Lifecycle_1.addDisposableDomListener(this.element, 'keypress', function (ev) {
            if (document.activeElement !== this) {
                return;
            }
            self._keyPress(ev);
        }, true));
        this.register(Lifecycle_1.addDisposableDomListener(this.element, 'keyup', function (ev) {
            if (!wasModifierKeyOnlyEvent(ev)) {
                _this.focus();
            }
            self._keyUp(ev);
        }, true));
        this.register(Lifecycle_1.addDisposableDomListener(this.textarea, 'keydown', function (ev) { return _this._keyDown(ev); }, true));
        this.register(Lifecycle_1.addDisposableDomListener(this.textarea, 'keypress', function (ev) { return _this._keyPress(ev); }, true));
        this.register(Lifecycle_1.addDisposableDomListener(this.textarea, 'compositionstart', function () { return _this._compositionHelper.compositionstart(); }));
        this.register(Lifecycle_1.addDisposableDomListener(this.textarea, 'compositionupdate', function (e) { return _this._compositionHelper.compositionupdate(e); }));
        this.register(Lifecycle_1.addDisposableDomListener(this.textarea, 'compositionend', function () { return _this._compositionHelper.compositionend(); }));
        this.register(this.onRender(function () { return _this._compositionHelper.updateCompositionElements(); }));
        this.register(this.onRender(function (e) { return _this._queueLinkification(e.start, e.end); }));
    };
    Terminal.prototype.open = function (parent) {
        var _this = this;
        this._parent = parent || this._parent;
        if (!this._parent) {
            throw new Error('Terminal requires a parent element.');
        }
        this._context = this._parent.ownerDocument.defaultView;
        this._document = this._parent.ownerDocument;
        this.element = this._document.createElement('div');
        this.element.dir = 'ltr';
        this.element.classList.add('terminal');
        this.element.classList.add('xterm');
        this.element.setAttribute('tabindex', '0');
        this._parent.appendChild(this.element);
        var fragment = document.createDocumentFragment();
        this._viewportElement = document.createElement('div');
        this._viewportElement.classList.add('xterm-viewport');
        fragment.appendChild(this._viewportElement);
        this._viewportScrollArea = document.createElement('div');
        this._viewportScrollArea.classList.add('xterm-scroll-area');
        this._viewportElement.appendChild(this._viewportScrollArea);
        this.screenElement = document.createElement('div');
        this.screenElement.classList.add('xterm-screen');
        this._helperContainer = document.createElement('div');
        this._helperContainer.classList.add('xterm-helpers');
        this.screenElement.appendChild(this._helperContainer);
        fragment.appendChild(this.screenElement);
        this._mouseZoneManager = new MouseZoneManager_1.MouseZoneManager(this);
        this.register(this._mouseZoneManager);
        this.register(this.onScroll(function () { return _this._mouseZoneManager.clearAll(); }));
        this.linkifier.attachToDom(this._mouseZoneManager);
        this.textarea = document.createElement('textarea');
        this.textarea.classList.add('xterm-helper-textarea');
        this.textarea.setAttribute('aria-label', Strings.promptLabel);
        this.textarea.setAttribute('aria-multiline', 'false');
        this.textarea.setAttribute('autocorrect', 'off');
        this.textarea.setAttribute('autocapitalize', 'off');
        this.textarea.setAttribute('spellcheck', 'false');
        this.textarea.tabIndex = 0;
        this.register(Lifecycle_1.addDisposableDomListener(this.textarea, 'focus', function (ev) { return _this._onTextAreaFocus(ev); }));
        this.register(Lifecycle_1.addDisposableDomListener(this.textarea, 'blur', function () { return _this._onTextAreaBlur(); }));
        this._helperContainer.appendChild(this.textarea);
        this._compositionView = document.createElement('div');
        this._compositionView.classList.add('composition-view');
        this._compositionHelper = new CompositionHelper_1.CompositionHelper(this.textarea, this._compositionView, this);
        this._helperContainer.appendChild(this._compositionView);
        this.charMeasure = new CharMeasure_1.CharMeasure(document, this._helperContainer);
        this.element.appendChild(fragment);
        this._theme = this.options.theme;
        this.options.theme = null;
        this._colorManager = new ColorManager_1.ColorManager(document, this.options.allowTransparency);
        this._colorManager.setTheme(this._theme);
        var renderer = this._createRenderer();
        this._renderCoordinator = new RenderCoordinator_1.RenderCoordinator(renderer, this.rows, this.screenElement);
        this._renderCoordinator.onRender(function (e) { return _this._onRender.fire(e); });
        this.onResize(function (e) { return _this._renderCoordinator.resize(e.cols, e.rows); });
        this.viewport = new Viewport_1.Viewport(this, this._viewportElement, this._viewportScrollArea, this.charMeasure, this._renderCoordinator.dimensions);
        this.viewport.onThemeChange(this._colorManager.colors);
        this.register(this.viewport);
        this.register(this.onCursorMove(function () { return _this._renderCoordinator.onCursorMove(); }));
        this.register(this.onResize(function () { return _this._renderCoordinator.onResize(_this.cols, _this.rows); }));
        this.register(this.addDisposableListener('blur', function () { return _this._renderCoordinator.onBlur(); }));
        this.register(this.addDisposableListener('focus', function () { return _this._renderCoordinator.onFocus(); }));
        this.register(this.charMeasure.onCharSizeChanged(function () { return _this._renderCoordinator.onCharSizeChanged(); }));
        this.register(this._renderCoordinator.onDimensionsChange(function () { return _this.viewport.syncScrollArea(); }));
        this.selectionManager = new SelectionManager_1.SelectionManager(this, this.charMeasure);
        this.register(this.selectionManager.onSelectionChange(function () { return _this._onSelectionChange.fire(); }));
        this.register(Lifecycle_1.addDisposableDomListener(this.element, 'mousedown', function (e) { return _this.selectionManager.onMouseDown(e); }));
        this.register(this.selectionManager.onRedrawRequest(function (e) { return _this._renderCoordinator.onSelectionChanged(e.start, e.end, e.columnSelectMode); }));
        this.register(this.selectionManager.onLinuxMouseSelection(function (text) {
            _this.textarea.value = text;
            _this.textarea.focus();
            _this.textarea.select();
        }));
        this.register(this.onScroll(function () {
            _this.viewport.syncScrollArea();
            _this.selectionManager.refresh();
        }));
        this.register(Lifecycle_1.addDisposableDomListener(this._viewportElement, 'scroll', function () { return _this.selectionManager.refresh(); }));
        this.mouseHelper = new MouseHelper_1.MouseHelper(this._renderCoordinator);
        this.element.classList.toggle('enable-mouse-events', this.mouseEvents);
        if (this.mouseEvents) {
            this.selectionManager.disable();
        }
        else {
            this.selectionManager.enable();
        }
        if (this.options.screenReaderMode) {
            this._accessibilityManager = new AccessibilityManager_1.AccessibilityManager(this, this._renderCoordinator.dimensions);
            this._accessibilityManager.register(this._renderCoordinator.onDimensionsChange(function (e) { return _this._accessibilityManager.setDimensions(e); }));
        }
        this.charMeasure.measure(this.options);
        this.refresh(0, this.rows - 1);
        this._initGlobal();
        this.bindMouse();
    };
    Terminal.prototype._createRenderer = function () {
        switch (this.options.rendererType) {
            case 'canvas':
                return new Renderer_1.Renderer(this, this._colorManager.colors);
                break;
            case 'dom':
                return new DomRenderer_1.DomRenderer(this, this._colorManager.colors);
                break;
            default: throw new Error("Unrecognized rendererType \"" + this.options.rendererType + "\"");
        }
    };
    Terminal.prototype._setTheme = function (theme) {
        this._theme = theme;
        this._colorManager.setTheme(theme);
        if (this._renderCoordinator) {
            this._renderCoordinator.setColors(this._colorManager.colors);
        }
        if (this.viewport) {
            this.viewport.onThemeChange(this._colorManager.colors);
        }
    };
    Terminal.prototype.bindMouse = function () {
        var _this = this;
        var el = this.element;
        var self = this;
        var pressed = 32;
        function sendButton(ev) {
            var button;
            var pos;
            button = getButton(ev);
            pos = self.mouseHelper.getRawByteCoords(ev, self.screenElement, self.charMeasure, self.cols, self.rows);
            if (!pos)
                return;
            sendEvent(button, pos);
            switch (ev.overrideType || ev.type) {
                case 'mousedown':
                    pressed = button;
                    break;
                case 'mouseup':
                    pressed = 32;
                    break;
                case 'wheel':
                    break;
            }
        }
        function sendMove(ev) {
            var button = pressed;
            var pos = self.mouseHelper.getRawByteCoords(ev, self.screenElement, self.charMeasure, self.cols, self.rows);
            if (!pos)
                return;
            button += 32;
            sendEvent(button, pos);
        }
        function encode(data, ch) {
            if (!self.utfMouse) {
                if (ch === 255) {
                    data.push(0);
                    return;
                }
                if (ch > 127)
                    ch = 127;
                data.push(ch);
            }
            else {
                if (ch > 2047) {
                    data.push(2047);
                    return;
                }
                data.push(ch);
            }
        }
        function sendEvent(button, pos) {
            if (self._vt300Mouse) {
                button &= 3;
                pos.x -= 32;
                pos.y -= 32;
                var data_1 = EscapeSequences_1.C0.ESC + '[24';
                if (button === 0)
                    data_1 += '1';
                else if (button === 1)
                    data_1 += '3';
                else if (button === 2)
                    data_1 += '5';
                else if (button === 3)
                    return;
                else
                    data_1 += '0';
                data_1 += '~[' + pos.x + ',' + pos.y + ']\r';
                self.handler(data_1);
                return;
            }
            if (self._decLocator) {
                button &= 3;
                pos.x -= 32;
                pos.y -= 32;
                if (button === 0)
                    button = 2;
                else if (button === 1)
                    button = 4;
                else if (button === 2)
                    button = 6;
                else if (button === 3)
                    button = 3;
                self.handler(EscapeSequences_1.C0.ESC + '['
                    + button
                    + ';'
                    + (button === 3 ? 4 : 0)
                    + ';'
                    + pos.y
                    + ';'
                    + pos.x
                    + ';'
                    + pos.page || 0
                    + '&w');
                return;
            }
            if (self.urxvtMouse) {
                pos.x -= 32;
                pos.y -= 32;
                pos.x++;
                pos.y++;
                self.handler(EscapeSequences_1.C0.ESC + '[' + button + ';' + pos.x + ';' + pos.y + 'M');
                return;
            }
            if (self.sgrMouse) {
                pos.x -= 32;
                pos.y -= 32;
                self.handler(EscapeSequences_1.C0.ESC + '[<'
                    + (((button & 3) === 3 ? button & ~3 : button) - 32)
                    + ';'
                    + pos.x
                    + ';'
                    + pos.y
                    + ((button & 3) === 3 ? 'm' : 'M'));
                return;
            }
            var data = [];
            encode(data, button);
            encode(data, pos.x);
            encode(data, pos.y);
            self.handler(EscapeSequences_1.C0.ESC + '[M' + String.fromCharCode.apply(String, data));
        }
        function getButton(ev) {
            var button;
            var shift;
            var meta;
            var ctrl;
            var mod;
            switch (ev.overrideType || ev.type) {
                case 'mousedown':
                    button = ev.button !== null && ev.button !== undefined
                        ? +ev.button
                        : ev.which !== null && ev.which !== undefined
                            ? ev.which - 1
                            : null;
                    if (Browser.isMSIE) {
                        button = button === 1 ? 0 : button === 4 ? 1 : button;
                    }
                    break;
                case 'mouseup':
                    button = 3;
                    break;
                case 'DOMMouseScroll':
                    button = ev.detail < 0
                        ? 64
                        : 65;
                    break;
                case 'wheel':
                    button = ev.deltaY < 0
                        ? 64
                        : 65;
                    break;
            }
            shift = ev.shiftKey ? 4 : 0;
            meta = ev.metaKey ? 8 : 0;
            ctrl = ev.ctrlKey ? 16 : 0;
            mod = shift | meta | ctrl;
            if (self.vt200Mouse) {
                mod &= ctrl;
            }
            else if (!self.normalMouse) {
                mod = 0;
            }
            button = (32 + (mod << 2)) + button;
            return button;
        }
        this.register(Lifecycle_1.addDisposableDomListener(el, 'mousedown', function (ev) {
            ev.preventDefault();
            _this.focus();
            if (!_this.mouseEvents || _this.selectionManager.shouldForceSelection(ev)) {
                return;
            }
            sendButton(ev);
            if (_this.vt200Mouse) {
                ev.overrideType = 'mouseup';
                sendButton(ev);
                return _this.cancel(ev);
            }
            var moveHandler;
            if (_this.normalMouse) {
                moveHandler = function (event) {
                    if (!_this.normalMouse) {
                        return;
                    }
                    sendMove(event);
                };
                _this._document.addEventListener('mousemove', moveHandler);
            }
            var handler = function (ev) {
                if (_this.normalMouse && !_this.x10Mouse) {
                    sendButton(ev);
                }
                if (moveHandler) {
                    _this._document.removeEventListener('mousemove', moveHandler);
                    moveHandler = null;
                }
                _this._document.removeEventListener('mouseup', handler);
                return _this.cancel(ev);
            };
            _this._document.addEventListener('mouseup', handler);
            return _this.cancel(ev);
        }));
        this.register(Lifecycle_1.addDisposableDomListener(el, 'wheel', function (ev) {
            if (!_this.mouseEvents) {
                if (!_this.buffer.hasScrollback) {
                    var amount = _this.viewport.getLinesScrolled(ev);
                    if (amount === 0) {
                        return;
                    }
                    var sequence = EscapeSequences_1.C0.ESC + (_this.applicationCursor ? 'O' : '[') + (ev.deltaY < 0 ? 'A' : 'B');
                    var data = '';
                    for (var i = 0; i < Math.abs(amount); i++) {
                        data += sequence;
                    }
                    _this.handler(data);
                }
                return;
            }
            if (_this.x10Mouse || _this._vt300Mouse || _this._decLocator)
                return;
            sendButton(ev);
            ev.preventDefault();
        }));
        this.register(Lifecycle_1.addDisposableDomListener(el, 'wheel', function (ev) {
            if (_this.mouseEvents)
                return;
            _this.viewport.onWheel(ev);
            return _this.cancel(ev);
        }));
        this.register(Lifecycle_1.addDisposableDomListener(el, 'touchstart', function (ev) {
            if (_this.mouseEvents)
                return;
            _this.viewport.onTouchStart(ev);
            return _this.cancel(ev);
        }));
        this.register(Lifecycle_1.addDisposableDomListener(el, 'touchmove', function (ev) {
            if (_this.mouseEvents)
                return;
            _this.viewport.onTouchMove(ev);
            return _this.cancel(ev);
        }));
    };
    Terminal.prototype.refresh = function (start, end) {
        if (this._renderCoordinator) {
            this._renderCoordinator.refreshRows(start, end);
        }
    };
    Terminal.prototype._queueLinkification = function (start, end) {
        if (this.linkifier) {
            this.linkifier.linkifyRows(start, end);
        }
    };
    Terminal.prototype.updateCursorStyle = function (ev) {
        if (this.selectionManager && this.selectionManager.shouldColumnSelect(ev)) {
            this.element.classList.add('column-select');
        }
        else {
            this.element.classList.remove('column-select');
        }
    };
    Terminal.prototype.showCursor = function () {
        if (!this.cursorState) {
            this.cursorState = 1;
            this.refresh(this.buffer.y, this.buffer.y);
        }
    };
    Terminal.prototype.scroll = function (isWrapped) {
        if (isWrapped === void 0) { isWrapped = false; }
        var newLine;
        newLine = this._blankLine;
        var eraseAttr = this.eraseAttrData();
        if (!newLine || newLine.length !== this.cols || newLine.getFg(0) !== eraseAttr.fg || newLine.getBg(0) !== eraseAttr.bg) {
            newLine = this.buffer.getBlankLine(eraseAttr, isWrapped);
            this._blankLine = newLine;
        }
        newLine.isWrapped = isWrapped;
        var topRow = this.buffer.ybase + this.buffer.scrollTop;
        var bottomRow = this.buffer.ybase + this.buffer.scrollBottom;
        if (this.buffer.scrollTop === 0) {
            var willBufferBeTrimmed = this.buffer.lines.isFull;
            if (bottomRow === this.buffer.lines.length - 1) {
                if (willBufferBeTrimmed) {
                    this.buffer.lines.recycle().copyFrom(newLine);
                }
                else {
                    this.buffer.lines.push(newLine.clone());
                }
            }
            else {
                this.buffer.lines.splice(bottomRow + 1, 0, newLine.clone());
            }
            if (!willBufferBeTrimmed) {
                this.buffer.ybase++;
                if (!this._userScrolling) {
                    this.buffer.ydisp++;
                }
            }
            else {
                if (this._userScrolling) {
                    this.buffer.ydisp = Math.max(this.buffer.ydisp - 1, 0);
                }
            }
        }
        else {
            var scrollRegionHeight = bottomRow - topRow + 1;
            this.buffer.lines.shiftElements(topRow + 1, scrollRegionHeight - 1, -1);
            this.buffer.lines.set(bottomRow, newLine.clone());
        }
        if (!this._userScrolling) {
            this.buffer.ydisp = this.buffer.ybase;
        }
        this.updateRange(this.buffer.scrollTop);
        this.updateRange(this.buffer.scrollBottom);
        this._onScroll.fire(this.buffer.ydisp);
    };
    Terminal.prototype.scrollLines = function (disp, suppressScrollEvent) {
        if (disp < 0) {
            if (this.buffer.ydisp === 0) {
                return;
            }
            this._userScrolling = true;
        }
        else if (disp + this.buffer.ydisp >= this.buffer.ybase) {
            this._userScrolling = false;
        }
        var oldYdisp = this.buffer.ydisp;
        this.buffer.ydisp = Math.max(Math.min(this.buffer.ydisp + disp, this.buffer.ybase), 0);
        if (oldYdisp === this.buffer.ydisp) {
            return;
        }
        if (!suppressScrollEvent) {
            this._onScroll.fire(this.buffer.ydisp);
        }
        this.refresh(0, this.rows - 1);
    };
    Terminal.prototype.scrollPages = function (pageCount) {
        this.scrollLines(pageCount * (this.rows - 1));
    };
    Terminal.prototype.scrollToTop = function () {
        this.scrollLines(-this.buffer.ydisp);
    };
    Terminal.prototype.scrollToBottom = function () {
        this.scrollLines(this.buffer.ybase - this.buffer.ydisp);
    };
    Terminal.prototype.scrollToLine = function (line) {
        var scrollAmount = line - this.buffer.ydisp;
        if (scrollAmount !== 0) {
            this.scrollLines(scrollAmount);
        }
    };
    Terminal.prototype.writeUtf8 = function (data) {
        var _this = this;
        if (this._isDisposed) {
            return;
        }
        if (!data) {
            return;
        }
        this.writeBufferUtf8.push(data);
        if (this.options.useFlowControl && !this._xoffSentToCatchUp && this.writeBufferUtf8.length >= WRITE_BUFFER_PAUSE_THRESHOLD) {
            this.handler(EscapeSequences_1.C0.DC3);
            this._xoffSentToCatchUp = true;
        }
        if (!this._writeInProgress && this.writeBufferUtf8.length > 0) {
            this._writeInProgress = true;
            setTimeout(function () {
                _this._innerWriteUtf8();
            });
        }
    };
    Terminal.prototype._innerWriteUtf8 = function (bufferOffset) {
        var _this = this;
        if (bufferOffset === void 0) { bufferOffset = 0; }
        if (this._isDisposed) {
            this.writeBufferUtf8 = [];
        }
        var startTime = Date.now();
        while (this.writeBufferUtf8.length > bufferOffset) {
            var data = this.writeBufferUtf8[bufferOffset];
            bufferOffset++;
            if (this._xoffSentToCatchUp && this.writeBufferUtf8.length === bufferOffset) {
                this.handler(EscapeSequences_1.C0.DC1);
                this._xoffSentToCatchUp = false;
            }
            this._refreshStart = this.buffer.y;
            this._refreshEnd = this.buffer.y;
            this._inputHandler.parseUtf8(data);
            this.updateRange(this.buffer.y);
            this.refresh(this._refreshStart, this._refreshEnd);
            if (Date.now() - startTime >= WRITE_TIMEOUT_MS) {
                break;
            }
        }
        if (this.writeBufferUtf8.length > bufferOffset) {
            if (bufferOffset > WRITE_BUFFER_LENGTH_THRESHOLD) {
                this.writeBufferUtf8 = this.writeBufferUtf8.slice(bufferOffset);
                bufferOffset = 0;
            }
            setTimeout(function () { return _this._innerWriteUtf8(bufferOffset); }, 0);
        }
        else {
            this._writeInProgress = false;
            this.writeBufferUtf8 = [];
        }
    };
    Terminal.prototype.write = function (data) {
        var _this = this;
        if (this._isDisposed) {
            return;
        }
        if (!data) {
            return;
        }
        this.writeBuffer.push(data);
        if (this.options.useFlowControl && !this._xoffSentToCatchUp && this.writeBuffer.length >= WRITE_BUFFER_PAUSE_THRESHOLD) {
            this.handler(EscapeSequences_1.C0.DC3);
            this._xoffSentToCatchUp = true;
        }
        if (!this._writeInProgress && this.writeBuffer.length > 0) {
            this._writeInProgress = true;
            setTimeout(function () {
                _this._innerWrite();
            });
        }
    };
    Terminal.prototype._innerWrite = function (bufferOffset) {
        var _this = this;
        if (bufferOffset === void 0) { bufferOffset = 0; }
        if (this._isDisposed) {
            this.writeBuffer = [];
        }
        var startTime = Date.now();
        while (this.writeBuffer.length > bufferOffset) {
            var data = this.writeBuffer[bufferOffset];
            bufferOffset++;
            if (this._xoffSentToCatchUp && this.writeBuffer.length === bufferOffset) {
                this.handler(EscapeSequences_1.C0.DC1);
                this._xoffSentToCatchUp = false;
            }
            this._refreshStart = this.buffer.y;
            this._refreshEnd = this.buffer.y;
            this._inputHandler.parse(data);
            this.updateRange(this.buffer.y);
            this.refresh(this._refreshStart, this._refreshEnd);
            if (Date.now() - startTime >= WRITE_TIMEOUT_MS) {
                break;
            }
        }
        if (this.writeBuffer.length > bufferOffset) {
            if (bufferOffset > WRITE_BUFFER_LENGTH_THRESHOLD) {
                this.writeBuffer = this.writeBuffer.slice(bufferOffset);
                bufferOffset = 0;
            }
            setTimeout(function () { return _this._innerWrite(bufferOffset); }, 0);
        }
        else {
            this._writeInProgress = false;
            this.writeBuffer = [];
        }
    };
    Terminal.prototype.writeln = function (data) {
        this.write(data + '\r\n');
    };
    Terminal.prototype.attachCustomKeyEventHandler = function (customKeyEventHandler) {
        this._customKeyEventHandler = customKeyEventHandler;
    };
    Terminal.prototype.addCsiHandler = function (flag, callback) {
        return this._inputHandler.addCsiHandler(flag, callback);
    };
    Terminal.prototype.addOscHandler = function (ident, callback) {
        return this._inputHandler.addOscHandler(ident, callback);
    };
    Terminal.prototype.registerLinkMatcher = function (regex, handler, options) {
        var matcherId = this.linkifier.registerLinkMatcher(regex, handler, options);
        this.refresh(0, this.rows - 1);
        return matcherId;
    };
    Terminal.prototype.deregisterLinkMatcher = function (matcherId) {
        if (this.linkifier.deregisterLinkMatcher(matcherId)) {
            this.refresh(0, this.rows - 1);
        }
    };
    Terminal.prototype.registerCharacterJoiner = function (handler) {
        var joinerId = this._renderCoordinator.registerCharacterJoiner(handler);
        this.refresh(0, this.rows - 1);
        return joinerId;
    };
    Terminal.prototype.deregisterCharacterJoiner = function (joinerId) {
        if (this._renderCoordinator.deregisterCharacterJoiner(joinerId)) {
            this.refresh(0, this.rows - 1);
        }
    };
    Object.defineProperty(Terminal.prototype, "markers", {
        get: function () {
            return this.buffer.markers;
        },
        enumerable: true,
        configurable: true
    });
    Terminal.prototype.addMarker = function (cursorYOffset) {
        if (this.buffer !== this.buffers.normal) {
            return;
        }
        return this.buffer.addMarker(this.buffer.ybase + this.buffer.y + cursorYOffset);
    };
    Terminal.prototype.hasSelection = function () {
        return this.selectionManager ? this.selectionManager.hasSelection : false;
    };
    Terminal.prototype.select = function (column, row, length) {
        this.selectionManager.setSelection(column, row, length);
    };
    Terminal.prototype.getSelection = function () {
        return this.selectionManager ? this.selectionManager.selectionText : '';
    };
    Terminal.prototype.getSelectionPosition = function () {
        if (!this.selectionManager.hasSelection) {
            return undefined;
        }
        return {
            startColumn: this.selectionManager.selectionStart[0],
            startRow: this.selectionManager.selectionStart[1],
            endColumn: this.selectionManager.selectionEnd[0],
            endRow: this.selectionManager.selectionEnd[1]
        };
    };
    Terminal.prototype.clearSelection = function () {
        if (this.selectionManager) {
            this.selectionManager.clearSelection();
        }
    };
    Terminal.prototype.selectAll = function () {
        if (this.selectionManager) {
            this.selectionManager.selectAll();
        }
    };
    Terminal.prototype.selectLines = function (start, end) {
        if (this.selectionManager) {
            this.selectionManager.selectLines(start, end);
        }
    };
    Terminal.prototype._keyDown = function (event) {
        if (this._customKeyEventHandler && this._customKeyEventHandler(event) === false) {
            return false;
        }
        if (!this._compositionHelper.keydown(event)) {
            if (this.buffer.ybase !== this.buffer.ydisp) {
                this.scrollToBottom();
            }
            return false;
        }
        var result = Keyboard_1.evaluateKeyboardEvent(event, this.applicationCursor, this.browser.isMac, this.options.macOptionIsMeta);
        this.updateCursorStyle(event);
        if (result.type === 3 || result.type === 2) {
            var scrollCount = this.rows - 1;
            this.scrollLines(result.type === 2 ? -scrollCount : scrollCount);
            return this.cancel(event, true);
        }
        if (result.type === 1) {
            this.selectAll();
        }
        if (this._isThirdLevelShift(this.browser, event)) {
            return true;
        }
        if (result.cancel) {
            this.cancel(event, true);
        }
        if (!result.key) {
            return true;
        }
        this.emit('keydown', event);
        this._onKey.fire({ key: result.key, domEvent: event });
        this.showCursor();
        this.handler(result.key);
        return this.cancel(event, true);
    };
    Terminal.prototype._isThirdLevelShift = function (browser, ev) {
        var thirdLevelKey = (browser.isMac && !this.options.macOptionIsMeta && ev.altKey && !ev.ctrlKey && !ev.metaKey) ||
            (browser.isMSWindows && ev.altKey && ev.ctrlKey && !ev.metaKey);
        if (ev.type === 'keypress') {
            return thirdLevelKey;
        }
        return thirdLevelKey && (!ev.keyCode || ev.keyCode > 47);
    };
    Terminal.prototype.setgLevel = function (g) {
        this.glevel = g;
        this.charset = this.charsets[g];
    };
    Terminal.prototype.setgCharset = function (g, charset) {
        this.charsets[g] = charset;
        if (this.glevel === g) {
            this.charset = charset;
        }
    };
    Terminal.prototype._keyUp = function (ev) {
        this.updateCursorStyle(ev);
    };
    Terminal.prototype._keyPress = function (ev) {
        var key;
        if (this._customKeyEventHandler && this._customKeyEventHandler(ev) === false) {
            return false;
        }
        this.cancel(ev);
        if (ev.charCode) {
            key = ev.charCode;
        }
        else if (ev.which === null || ev.which === undefined) {
            key = ev.keyCode;
        }
        else if (ev.which !== 0 && ev.charCode !== 0) {
            key = ev.which;
        }
        else {
            return false;
        }
        if (!key || ((ev.altKey || ev.ctrlKey || ev.metaKey) && !this._isThirdLevelShift(this.browser, ev))) {
            return false;
        }
        key = String.fromCharCode(key);
        this.emit('keypress', key, ev);
        this._onKey.fire({ key: key, domEvent: ev });
        this.showCursor();
        this.handler(key);
        return true;
    };
    Terminal.prototype.bell = function () {
        var _this = this;
        this.emit('bell');
        if (this._soundBell()) {
            this.soundManager.playBellSound();
        }
        if (this._visualBell()) {
            this.element.classList.add('visual-bell-active');
            clearTimeout(this._visualBellTimer);
            this._visualBellTimer = window.setTimeout(function () {
                _this.element.classList.remove('visual-bell-active');
            }, 200);
        }
    };
    Terminal.prototype.log = function (text, data) {
        if (!this.options.debug)
            return;
        if (!this._context.console || !this._context.console.log)
            return;
        this._context.console.log(text, data);
    };
    Terminal.prototype.error = function (text, data) {
        if (!this.options.debug)
            return;
        if (!this._context.console || !this._context.console.error)
            return;
        this._context.console.error(text, data);
    };
    Terminal.prototype.resize = function (x, y) {
        if (isNaN(x) || isNaN(y)) {
            return;
        }
        if (x === this.cols && y === this.rows) {
            if (this.charMeasure && (!this.charMeasure.width || !this.charMeasure.height)) {
                this.charMeasure.measure(this.options);
            }
            return;
        }
        if (x < MINIMUM_COLS)
            x = MINIMUM_COLS;
        if (y < MINIMUM_ROWS)
            y = MINIMUM_ROWS;
        this.buffers.resize(x, y);
        this.cols = x;
        this.rows = y;
        this.buffers.setupTabStops(this.cols);
        if (this.charMeasure) {
            this.charMeasure.measure(this.options);
        }
        this.refresh(0, this.rows - 1);
        this._onResize.fire({ cols: x, rows: y });
    };
    Terminal.prototype.updateRange = function (y) {
        if (y < this._refreshStart)
            this._refreshStart = y;
        if (y > this._refreshEnd)
            this._refreshEnd = y;
    };
    Terminal.prototype.maxRange = function () {
        this._refreshStart = 0;
        this._refreshEnd = this.rows - 1;
    };
    Terminal.prototype.clear = function () {
        if (this.buffer.ybase === 0 && this.buffer.y === 0) {
            return;
        }
        this.buffer.lines.set(0, this.buffer.lines.get(this.buffer.ybase + this.buffer.y));
        this.buffer.lines.length = 1;
        this.buffer.ydisp = 0;
        this.buffer.ybase = 0;
        this.buffer.y = 0;
        for (var i = 1; i < this.rows; i++) {
            this.buffer.lines.push(this.buffer.getBlankLine(BufferLine_1.DEFAULT_ATTR_DATA));
        }
        this.refresh(0, this.rows - 1);
        this._onScroll.fire(this.buffer.ydisp);
    };
    Terminal.prototype.is = function (term) {
        return (this.options.termName + '').indexOf(term) === 0;
    };
    Terminal.prototype.handler = function (data) {
        if (this.options.disableStdin) {
            return;
        }
        if (this.selectionManager && this.selectionManager.hasSelection) {
            this.selectionManager.clearSelection();
        }
        if (this.buffer.ybase !== this.buffer.ydisp) {
            this.scrollToBottom();
        }
        this._onData.fire(data);
    };
    Terminal.prototype.handleTitle = function (title) {
        this._onTitleChange.fire(title);
    };
    Terminal.prototype.index = function () {
        this.buffer.y++;
        if (this.buffer.y > this.buffer.scrollBottom) {
            this.buffer.y--;
            this.scroll();
        }
        if (this.buffer.x >= this.cols) {
            this.buffer.x--;
        }
    };
    Terminal.prototype.reverseIndex = function () {
        if (this.buffer.y === this.buffer.scrollTop) {
            var scrollRegionHeight = this.buffer.scrollBottom - this.buffer.scrollTop;
            this.buffer.lines.shiftElements(this.buffer.y + this.buffer.ybase, scrollRegionHeight, 1);
            this.buffer.lines.set(this.buffer.y + this.buffer.ybase, this.buffer.getBlankLine(this.eraseAttrData()));
            this.updateRange(this.buffer.scrollTop);
            this.updateRange(this.buffer.scrollBottom);
        }
        else {
            this.buffer.y--;
        }
    };
    Terminal.prototype.reset = function () {
        this.options.rows = this.rows;
        this.options.cols = this.cols;
        var customKeyEventHandler = this._customKeyEventHandler;
        var inputHandler = this._inputHandler;
        var cursorState = this.cursorState;
        var writeBuffer = this.writeBuffer;
        var writeBufferUtf8 = this.writeBufferUtf8;
        var writeInProgress = this._writeInProgress;
        var xoffSentToCatchUp = this._xoffSentToCatchUp;
        var userScrolling = this._userScrolling;
        this._setup();
        this._customKeyEventHandler = customKeyEventHandler;
        this._inputHandler = inputHandler;
        this.cursorState = cursorState;
        this.writeBuffer = writeBuffer;
        this.writeBufferUtf8 = writeBufferUtf8;
        this._writeInProgress = writeInProgress;
        this._xoffSentToCatchUp = xoffSentToCatchUp;
        this._userScrolling = userScrolling;
        this.refresh(0, this.rows - 1);
        if (this.viewport) {
            this.viewport.syncScrollArea();
        }
    };
    Terminal.prototype.tabSet = function () {
        this.buffer.tabs[this.buffer.x] = true;
    };
    Terminal.prototype.cancel = function (ev, force) {
        if (!this.options.cancelEvents && !force) {
            return;
        }
        ev.preventDefault();
        ev.stopPropagation();
        return false;
    };
    Terminal.prototype._visualBell = function () {
        return false;
    };
    Terminal.prototype._soundBell = function () {
        return this.options.bellStyle === 'sound';
    };
    return Terminal;
}(EventEmitter_1.EventEmitter));
exports.Terminal = Terminal;
function wasModifierKeyOnlyEvent(ev) {
    return ev.keyCode === 16 ||
        ev.keyCode === 17 ||
        ev.keyCode === 18;
}
//# sourceMappingURL=Terminal.js.map