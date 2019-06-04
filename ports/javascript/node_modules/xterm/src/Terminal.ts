/**
 * Copyright (c) 2014 The xterm.js authors. All rights reserved.
 * Copyright (c) 2012-2013, Christopher Jeffrey (MIT License)
 * @license MIT
 *
 * Originally forked from (with the author's permission):
 *   Fabrice Bellard's javascript vt100 for jslinux:
 *   http://bellard.org/jslinux/
 *   Copyright (c) 2011 Fabrice Bellard
 *   The original design remains. The terminal itself
 *   has been extended to include xterm CSI codes, among
 *   other features.
 *
 * Terminal Emulation References:
 *   http://vt100.net/
 *   http://invisible-island.net/xterm/ctlseqs/ctlseqs.txt
 *   http://invisible-island.net/xterm/ctlseqs/ctlseqs.html
 *   http://invisible-island.net/vttest/
 *   http://www.inwap.com/pdp10/ansicode.txt
 *   http://linux.die.net/man/4/console_codes
 *   http://linux.die.net/man/7/urxvt
 */

import { IInputHandlingTerminal, IViewport, ICompositionHelper, ITerminalOptions, ITerminal, IBrowser, ILinkifier, ILinkMatcherOptions, CustomKeyEventHandler, LinkMatcherHandler, CharacterJoinerHandler, IMouseZoneManager } from './Types';
import { IRenderer } from './renderer/Types';
import { BufferSet } from './BufferSet';
import { Buffer, MAX_BUFFER_SIZE } from './Buffer';
import { CompositionHelper } from './CompositionHelper';
import { EventEmitter } from './common/EventEmitter';
import { Viewport } from './Viewport';
import { rightClickHandler, moveTextAreaUnderMouseCursor, pasteHandler, copyHandler } from './Clipboard';
import { C0 } from './common/data/EscapeSequences';
import { InputHandler } from './InputHandler';
import { Renderer } from './renderer/Renderer';
import { Linkifier } from './Linkifier';
import { SelectionManager } from './SelectionManager';
import { CharMeasure } from './CharMeasure';
import * as Browser from './common/Platform';
import { addDisposableDomListener } from './ui/Lifecycle';
import * as Strings from './Strings';
import { MouseHelper } from './MouseHelper';
import { DEFAULT_BELL_SOUND, SoundManager } from './SoundManager';
import { MouseZoneManager } from './MouseZoneManager';
import { AccessibilityManager } from './AccessibilityManager';
import { ITheme, IMarker, IDisposable, ISelectionPosition } from 'xterm';
import { removeTerminalFromCache } from './renderer/atlas/CharAtlasCache';
import { DomRenderer } from './renderer/dom/DomRenderer';
import { IKeyboardEvent } from './common/Types';
import { evaluateKeyboardEvent } from './core/input/Keyboard';
import { KeyboardResultType, ICharset, IBufferLine, IAttributeData } from './core/Types';
import { clone } from './common/Clone';
import { EventEmitter2, IEvent } from './common/EventEmitter2';
import { Attributes, DEFAULT_ATTR_DATA } from './core/buffer/BufferLine';
import { applyWindowsMode } from './WindowsMode';
import { ColorManager } from './ui/ColorManager';
import { RenderCoordinator } from './renderer/RenderCoordinator';

// Let it work inside Node.js for automated testing purposes.
const document = (typeof window !== 'undefined') ? window.document : null;

/**
 * The amount of write requests to queue before sending an XOFF signal to the
 * pty process. This number must be small in order for ^C and similar sequences
 * to be responsive.
 */
const WRITE_BUFFER_PAUSE_THRESHOLD = 5;

/**
 * The max number of ms to spend on writes before allowing the renderer to
 * catch up with a 0ms setTimeout. A value of < 33 to keep us close to
 * 30fps, and a value of < 16 to try to run at 60fps. Of course, the real FPS
 * depends on the time it takes for the renderer to draw the frame.
 */
const WRITE_TIMEOUT_MS = 12;
const WRITE_BUFFER_LENGTH_THRESHOLD = 50;

const MINIMUM_COLS = 2; // Less than 2 can mess with wide chars
const MINIMUM_ROWS = 1;

/**
 * The set of options that only have an effect when set in the Terminal constructor.
 */
const CONSTRUCTOR_ONLY_OPTIONS = ['cols', 'rows'];

const DEFAULT_OPTIONS: ITerminalOptions = {
  cols: 80,
  rows: 24,
  convertEol: false,
  termName: 'xterm',
  cursorBlink: false,
  cursorStyle: 'block',
  bellSound: DEFAULT_BELL_SOUND,
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

export class Terminal extends EventEmitter implements ITerminal, IDisposable, IInputHandlingTerminal {
  public textarea: HTMLTextAreaElement;
  public element: HTMLElement;
  public screenElement: HTMLElement;

  /**
   * The HTMLElement that the terminal is created in, set by Terminal.open.
   */
  private _parent: HTMLElement;
  private _context: Window;
  private _document: Document;
  private _viewportScrollArea: HTMLElement;
  private _viewportElement: HTMLElement;
  private _helperContainer: HTMLElement;
  private _compositionView: HTMLElement;

  private _visualBellTimer: number;

  public browser: IBrowser = <any>Browser;

  public options: ITerminalOptions;

  // TODO: This can be changed to an enum or boolean, 0 and 1 seem to be the only options
  public cursorState: number;
  public cursorHidden: boolean;

  private _customKeyEventHandler: CustomKeyEventHandler;

  // modes
  public applicationKeypad: boolean;
  public applicationCursor: boolean;
  public originMode: boolean;
  public insertMode: boolean;
  public wraparoundMode: boolean; // defaults: xterm - true, vt100 - false
  public bracketedPasteMode: boolean;

  // charset
  // The current charset
  public charset: ICharset;
  public gcharset: number;
  public glevel: number;
  public charsets: ICharset[];

  // mouse properties
  private _decLocator: boolean; // This is unstable and never set
  public x10Mouse: boolean;
  public vt200Mouse: boolean;
  private _vt300Mouse: boolean; // This is unstable and never set
  public normalMouse: boolean;
  public mouseEvents: boolean;
  public sendFocus: boolean;
  public utfMouse: boolean;
  public sgrMouse: boolean;
  public urxvtMouse: boolean;

  // misc
  private _refreshStart: number;
  private _refreshEnd: number;
  public savedCols: number;

  public curAttrData: IAttributeData;
  private _eraseAttrData: IAttributeData;

  public params: (string | number)[];
  public currentParam: string | number;

  // user input states
  public writeBuffer: string[];
  public writeBufferUtf8: Uint8Array[];
  private _writeInProgress: boolean;

  /**
   * Whether _xterm.js_ sent XOFF in order to catch up with the pty process.
   * This is a distinct state from writeStopped so that if the user requested
   * XOFF via ^S that it will not automatically resume when the writeBuffer goes
   * below threshold.
   */
  private _xoffSentToCatchUp: boolean;

  /** Whether writing has been stopped as a result of XOFF */
  // private _writeStopped: boolean;

  // Store if user went browsing history in scrollback
  private _userScrolling: boolean;

  private _inputHandler: InputHandler;
  public soundManager: SoundManager;
  private _renderCoordinator: RenderCoordinator;
  public selectionManager: SelectionManager;
  public linkifier: ILinkifier;
  public buffers: BufferSet;
  public viewport: IViewport;
  private _compositionHelper: ICompositionHelper;
  public charMeasure: CharMeasure;
  private _mouseZoneManager: IMouseZoneManager;
  public mouseHelper: MouseHelper;
  private _accessibilityManager: AccessibilityManager;
  private _colorManager: ColorManager;
  private _theme: ITheme;
  private _windowsMode: IDisposable | undefined;

  // bufferline to clone/copy from for new blank lines
  private _blankLine: IBufferLine = null;

  public cols: number;
  public rows: number;

  private _onCursorMove = new EventEmitter2<void>();
  public get onCursorMove(): IEvent<void> { return this._onCursorMove.event; }
  private _onData = new EventEmitter2<string>();
  public get onData(): IEvent<string> { return this._onData.event; }
  private _onKey = new EventEmitter2<{ key: string, domEvent: KeyboardEvent }>();
  public get onKey(): IEvent<{ key: string, domEvent: KeyboardEvent }> { return this._onKey.event; }
  private _onLineFeed = new EventEmitter2<void>();
  public get onLineFeed(): IEvent<void> { return this._onLineFeed.event; }
  private _onRender = new EventEmitter2<{ start: number, end: number }>();
  public get onRender(): IEvent<{ start: number, end: number }> { return this._onRender.event; }
  private _onResize = new EventEmitter2<{ cols: number, rows: number }>();
  public get onResize(): IEvent<{ cols: number, rows: number }> { return this._onResize.event; }
  private _onScroll = new EventEmitter2<number>();
  public get onScroll(): IEvent<number> { return this._onScroll.event; }
  private _onSelectionChange = new EventEmitter2<void>();
  public get onSelectionChange(): IEvent<void> { return this._onSelectionChange.event; }
  private _onTitleChange = new EventEmitter2<string>();
  public get onTitleChange(): IEvent<string> { return this._onTitleChange.event; }

  /**
   * Creates a new `Terminal` object.
   *
   * @param options An object containing a set of options, the available options are:
   *   - `cursorBlink` (boolean): Whether the terminal cursor blinks
   *   - `cols` (number): The number of columns of the terminal (horizontal size)
   *   - `rows` (number): The number of rows of the terminal (vertical size)
   *
   * @public
   * @class Xterm Xterm
   * @alias module:xterm/src/xterm
   */
  constructor(
    options: ITerminalOptions = {}
  ) {
    super();
    this.options = clone(options);
    this._setup();

    // TODO: Remove these in v4
    // Fire old style events from new emitters
    this.onCursorMove(() => this.emit('cursormove'));
    this.onData(e => this.emit('data', e));
    this.onKey(e => this.emit('key', e.key, e.domEvent));
    this.onLineFeed(() => this.emit('linefeed'));
    this.onRender(e => this.emit('refresh', e));
    this.onResize(e => this.emit('resize', e));
    this.onSelectionChange(() => this.emit('selection'));
    this.onScroll(e => this.emit('scroll', e));
    this.onTitleChange(e => this.emit('title', e));
  }

  public dispose(): void {
    super.dispose();
    if (this._windowsMode) {
      this._windowsMode.dispose();
      this._windowsMode = undefined;
    }
    this._customKeyEventHandler = null;
    removeTerminalFromCache(this);
    this.handler = () => {};
    this.write = () => {};
    if (this.element && this.element.parentNode) {
      this.element.parentNode.removeChild(this.element);
    }
  }

  /**
   * @deprecated Use dispose instead.
   */
  public destroy(): void {
    this.dispose();
  }

  private _setup(): void {
    Object.keys(DEFAULT_OPTIONS).forEach((key) => {
      if (this.options[key] === null || this.options[key] === undefined) {
        this.options[key] = DEFAULT_OPTIONS[key];
      }
    });

    // this.context = options.context || window;
    // this.document = options.document || document;
    // TODO: WHy not document.body?
    this._parent = document ? document.body : null;

    this.cols = Math.max(this.options.cols, MINIMUM_COLS);
    this.rows = Math.max(this.options.rows, MINIMUM_ROWS);

    if (this.options.handler) {
      this.onData(this.options.handler);
    }

    this.cursorState = 0;
    this.cursorHidden = false;
    this._customKeyEventHandler = null;

    // modes
    this.applicationKeypad = false;
    this.applicationCursor = false;
    this.originMode = false;
    this.insertMode = false;
    this.wraparoundMode = true; // defaults: xterm - true, vt100 - false
    this.bracketedPasteMode = false;

    // charset
    this.charset = null;
    this.gcharset = null;
    this.glevel = 0;
    // TODO: Can this be just []?
    this.charsets = [null];

    this.curAttrData = DEFAULT_ATTR_DATA.clone();
    this._eraseAttrData = DEFAULT_ATTR_DATA.clone();

    this.params = [];
    this.currentParam = 0;

    // user input states
    this.writeBuffer = [];
    this.writeBufferUtf8 = [];
    this._writeInProgress = false;

    this._xoffSentToCatchUp = false;
    // this._writeStopped = false;
    this._userScrolling = false;

    // Register input handler and refire/handle events
    this._inputHandler = new InputHandler(this);
    this._inputHandler.onCursorMove(() => this._onCursorMove.fire());
    this._inputHandler.onLineFeed(() => this._onLineFeed.fire());
    this._inputHandler.onData(e => this._onData.fire(e));
    this.register(this._inputHandler);

    this.selectionManager = this.selectionManager || null;
    this.linkifier = this.linkifier || new Linkifier(this);
    this._mouseZoneManager = this._mouseZoneManager || null;
    this.soundManager = this.soundManager || new SoundManager(this);

    // Create the terminal's buffers and set the current buffer
    this.buffers = new BufferSet(this);
    if (this.selectionManager) {
      this.selectionManager.clearSelection();
      this.selectionManager.initBuffersListeners();
    }

    if (this.options.windowsMode) {
      this._windowsMode = applyWindowsMode(this);
    }
  }

  /**
   * Convenience property to active buffer.
   */
  public get buffer(): Buffer {
    return this.buffers.active;
  }

  /**
   * back_color_erase feature for xterm.
   */
  public eraseAttrData(): IAttributeData {
    this._eraseAttrData.bg &= ~(Attributes.CM_MASK | 0xFFFFFF);
    this._eraseAttrData.bg |= this.curAttrData.bg & ~0xFC000000;
    return this._eraseAttrData;
  }

  /**
   * Focus the terminal. Delegates focus handling to the terminal's DOM element.
   */
  public focus(): void {
    if (this.textarea) {
      this.textarea.focus({ preventScroll: true });
    }
  }

  public get isFocused(): boolean {
    return document.activeElement === this.textarea && document.hasFocus();
  }

  /**
   * Retrieves an option's value from the terminal.
   * @param key The option key.
   */
  public getOption(key: string): any {
    if (!(key in DEFAULT_OPTIONS)) {
      throw new Error('No option with key "' + key + '"');
    }

    return this.options[key];
  }

  /**
   * Sets an option on the terminal.
   * @param key The option key.
   * @param value The option value.
   */
  public setOption(key: string, value: any): void {
    if (!(key in DEFAULT_OPTIONS)) {
      throw new Error('No option with key "' + key + '"');
    }
    if (CONSTRUCTOR_ONLY_OPTIONS.indexOf(key) !== -1) {
      console.error(`Option "${key}" can only be set in the constructor`);
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
          console.warn(`${key} cannot be less than 1, value: ${value}`);
          return;
        }
      case 'rendererType':
        if (!value) {
          value = 'canvas';
        }
        break;
      case 'tabStopWidth':
        if (value < 1) {
          console.warn(`${key} cannot be less than 1, value: ${value}`);
          return;
        }
        break;
      case 'theme':
        this._setTheme(<ITheme>value);
        break;
      case 'scrollback':
        value = Math.min(value, MAX_BUFFER_SIZE);

        if (value < 0) {
          console.warn(`${key} cannot be less than 0, value: ${value}`);
          return;
        }
        if (this.options[key] !== value) {
          const newBufferLength = this.rows + value;
          if (this.buffer.lines.length > newBufferLength) {
            const amountToTrim = this.buffer.lines.length - newBufferLength;
            const needsRefresh = (this.buffer.ydisp - amountToTrim < 0);
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
        // When the font changes the size of the cells may change which requires a renderer clear
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
        // When the font changes the size of the cells may change which requires a renderer clear
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
            this._accessibilityManager = new AccessibilityManager(this, this._renderCoordinator.dimensions);
          }
        } else {
          if (this._accessibilityManager) {
            this._accessibilityManager.dispose();
            this._accessibilityManager = null;
          }
        }
        break;
      case 'tabStopWidth': this.buffers.setupTabStops(); break;
      case 'windowsMode':
        if (value) {
          if (!this._windowsMode) {
            this._windowsMode = applyWindowsMode(this);
          }
        } else {
          if (this._windowsMode) {
            this._windowsMode.dispose();
            this._windowsMode = undefined;
          }
        }
        break;
    }
    // Inform renderer of changes
    if (this._renderCoordinator) {
      this._renderCoordinator.onOptionsChanged();
    }
  }

  /**
   * Binds the desired focus behavior on a given terminal object.
   */
  private _onTextAreaFocus(ev: KeyboardEvent): void {
    if (this.sendFocus) {
      this.handler(C0.ESC + '[I');
    }
    this.updateCursorStyle(ev);
    this.element.classList.add('focus');
    this.showCursor();
    this.emit('focus');
  }

  /**
   * Blur the terminal, calling the blur function on the terminal's underlying
   * textarea.
   */
  public blur(): void {
    return this.textarea.blur();
  }

  /**
   * Binds the desired blur behavior on a given terminal object.
   */
  private _onTextAreaBlur(): void {
    // Text can safely be removed on blur. Doing it earlier could interfere with
    // screen readers reading it out.
    this.textarea.value = '';
    this.refresh(this.buffer.y, this.buffer.y);
    if (this.sendFocus) {
      this.handler(C0.ESC + '[O');
    }
    this.element.classList.remove('focus');
    this.emit('blur');
  }

  /**
   * Initialize default behavior
   */
  private _initGlobal(): void {
    this._bindKeys();

    // Bind clipboard functionality
    this.register(addDisposableDomListener(this.element, 'copy', (event: ClipboardEvent) => {
      // If mouse events are active it means the selection manager is disabled and
      // copy should be handled by the host program.
      if (!this.hasSelection()) {
        return;
      }
      copyHandler(event, this, this.selectionManager);
    }));
    const pasteHandlerWrapper = (event: ClipboardEvent) => pasteHandler(event, this);
    this.register(addDisposableDomListener(this.textarea, 'paste', pasteHandlerWrapper));
    this.register(addDisposableDomListener(this.element, 'paste', pasteHandlerWrapper));

    // Handle right click context menus
    if (Browser.isFirefox) {
      // Firefox doesn't appear to fire the contextmenu event on right click
      this.register(addDisposableDomListener(this.element, 'mousedown', (event: MouseEvent) => {
        if (event.button === 2) {
          rightClickHandler(event, this, this.selectionManager, this.options.rightClickSelectsWord);
        }
      }));
    } else {
      this.register(addDisposableDomListener(this.element, 'contextmenu', (event: MouseEvent) => {
        rightClickHandler(event, this, this.selectionManager, this.options.rightClickSelectsWord);
      }));
    }

    // Move the textarea under the cursor when middle clicking on Linux to ensure
    // middle click to paste selection works. This only appears to work in Chrome
    // at the time is writing.
    if (Browser.isLinux) {
      // Use auxclick event over mousedown the latter doesn't seem to work. Note
      // that the regular click event doesn't fire for the middle mouse button.
      this.register(addDisposableDomListener(this.element, 'auxclick', (event: MouseEvent) => {
        if (event.button === 1) {
          moveTextAreaUnderMouseCursor(event, this);
        }
      }));
    }
  }

  /**
   * Apply key handling to the terminal
   */
  private _bindKeys(): void {
    const self = this;
    this.register(addDisposableDomListener(this.element, 'keydown', function (ev: KeyboardEvent): void {
      if (document.activeElement !== this) {
        return;
      }
      self._keyDown(ev);
    }, true));

    this.register(addDisposableDomListener(this.element, 'keypress', function (ev: KeyboardEvent): void {
      if (document.activeElement !== this) {
        return;
      }
      self._keyPress(ev);
    }, true));

    this.register(addDisposableDomListener(this.element, 'keyup', (ev: KeyboardEvent) => {
      if (!wasModifierKeyOnlyEvent(ev)) {
        this.focus();
      }

      self._keyUp(ev);
    }, true));

    this.register(addDisposableDomListener(this.textarea, 'keydown', (ev: KeyboardEvent) => this._keyDown(ev), true));
    this.register(addDisposableDomListener(this.textarea, 'keypress', (ev: KeyboardEvent) => this._keyPress(ev), true));
    this.register(addDisposableDomListener(this.textarea, 'compositionstart', () => this._compositionHelper.compositionstart()));
    this.register(addDisposableDomListener(this.textarea, 'compositionupdate', (e: CompositionEvent) => this._compositionHelper.compositionupdate(e)));
    this.register(addDisposableDomListener(this.textarea, 'compositionend', () => this._compositionHelper.compositionend()));
    this.register(this.onRender(() => this._compositionHelper.updateCompositionElements()));
    this.register(this.onRender(e => this._queueLinkification(e.start, e.end)));
  }

  /**
   * Opens the terminal within an element.
   *
   * @param parent The element to create the terminal within.
   */
  public open(parent: HTMLElement): void {
    this._parent = parent || this._parent;

    if (!this._parent) {
      throw new Error('Terminal requires a parent element.');
    }

    // Grab global elements
    this._context = this._parent.ownerDocument.defaultView;
    this._document = this._parent.ownerDocument;

    // Create main element container
    this.element = this._document.createElement('div');
    this.element.dir = 'ltr';   // xterm.css assumes LTR
    this.element.classList.add('terminal');
    this.element.classList.add('xterm');
    this.element.setAttribute('tabindex', '0');
    this._parent.appendChild(this.element);

    // Performance: Use a document fragment to build the terminal
    // viewport and helper elements detached from the DOM
    const fragment = document.createDocumentFragment();
    this._viewportElement = document.createElement('div');
    this._viewportElement.classList.add('xterm-viewport');
    fragment.appendChild(this._viewportElement);
    this._viewportScrollArea = document.createElement('div');
    this._viewportScrollArea.classList.add('xterm-scroll-area');
    this._viewportElement.appendChild(this._viewportScrollArea);

    this.screenElement = document.createElement('div');
    this.screenElement.classList.add('xterm-screen');
    // Create the container that will hold helpers like the textarea for
    // capturing DOM Events. Then produce the helpers.
    this._helperContainer = document.createElement('div');
    this._helperContainer.classList.add('xterm-helpers');
    this.screenElement.appendChild(this._helperContainer);
    fragment.appendChild(this.screenElement);

    this._mouseZoneManager = new MouseZoneManager(this);
    this.register(this._mouseZoneManager);
    this.register(this.onScroll(() => this._mouseZoneManager.clearAll()));
    this.linkifier.attachToDom(this._mouseZoneManager);

    this.textarea = document.createElement('textarea');
    this.textarea.classList.add('xterm-helper-textarea');
    // TODO: New API to set title? This could say "Terminal bash input", etc.
    this.textarea.setAttribute('aria-label', Strings.promptLabel);
    this.textarea.setAttribute('aria-multiline', 'false');
    this.textarea.setAttribute('autocorrect', 'off');
    this.textarea.setAttribute('autocapitalize', 'off');
    this.textarea.setAttribute('spellcheck', 'false');
    this.textarea.tabIndex = 0;
    this.register(addDisposableDomListener(this.textarea, 'focus', (ev: KeyboardEvent) => this._onTextAreaFocus(ev)));
    this.register(addDisposableDomListener(this.textarea, 'blur', () => this._onTextAreaBlur()));
    this._helperContainer.appendChild(this.textarea);

    this._compositionView = document.createElement('div');
    this._compositionView.classList.add('composition-view');
    this._compositionHelper = new CompositionHelper(this.textarea, this._compositionView, this);
    this._helperContainer.appendChild(this._compositionView);

    this.charMeasure = new CharMeasure(document, this._helperContainer);

    // Performance: Add viewport and helper elements from the fragment
    this.element.appendChild(fragment);

    this._theme = this.options.theme;
    this.options.theme = null;
    this._colorManager = new ColorManager(document, this.options.allowTransparency);
    this._colorManager.setTheme(this._theme);

    const renderer = this._createRenderer();
    this._renderCoordinator = new RenderCoordinator(renderer, this.rows, this.screenElement);
    this._renderCoordinator.onRender(e => this._onRender.fire(e));
    this.onResize(e => this._renderCoordinator.resize(e.cols, e.rows));

    this.viewport = new Viewport(this, this._viewportElement, this._viewportScrollArea, this.charMeasure, this._renderCoordinator.dimensions);
    this.viewport.onThemeChange(this._colorManager.colors);
    this.register(this.viewport);

    this.register(this.onCursorMove(() => this._renderCoordinator.onCursorMove()));
    this.register(this.onResize(() => this._renderCoordinator.onResize(this.cols, this.rows)));
    this.register(this.addDisposableListener('blur', () => this._renderCoordinator.onBlur()));
    this.register(this.addDisposableListener('focus', () => this._renderCoordinator.onFocus()));
    this.register(this.charMeasure.onCharSizeChanged(() => this._renderCoordinator.onCharSizeChanged()));
    this.register(this._renderCoordinator.onDimensionsChange(() => this.viewport.syncScrollArea()));

    this.selectionManager = new SelectionManager(this, this.charMeasure);
    this.register(this.selectionManager.onSelectionChange(() => this._onSelectionChange.fire()));
    this.register(addDisposableDomListener(this.element, 'mousedown', (e: MouseEvent) => this.selectionManager.onMouseDown(e)));
    this.register(this.selectionManager.onRedrawRequest(e => this._renderCoordinator.onSelectionChanged(e.start, e.end, e.columnSelectMode)));
    this.register(this.selectionManager.onLinuxMouseSelection(text => {
      // If there's a new selection, put it into the textarea, focus and select it
      // in order to register it as a selection on the OS. This event is fired
      // only on Linux to enable middle click to paste selection.
      this.textarea.value = text;
      this.textarea.focus();
      this.textarea.select();
    }));
    this.register(this.onScroll(() => {
      this.viewport.syncScrollArea();
      this.selectionManager.refresh();
    }));
    this.register(addDisposableDomListener(this._viewportElement, 'scroll', () => this.selectionManager.refresh()));

    this.mouseHelper = new MouseHelper(this._renderCoordinator);
    // apply mouse event classes set by escape codes before terminal was attached
    this.element.classList.toggle('enable-mouse-events', this.mouseEvents);
    if (this.mouseEvents) {
      this.selectionManager.disable();
    } else {
      this.selectionManager.enable();
    }

    if (this.options.screenReaderMode) {
      // Note that this must be done *after* the renderer is created in order to
      // ensure the correct order of the dprchange event
      this._accessibilityManager = new AccessibilityManager(this, this._renderCoordinator.dimensions);
      this._accessibilityManager.register(this._renderCoordinator.onDimensionsChange(e => this._accessibilityManager.setDimensions(e)));
    }

    // Measure the character size
    this.charMeasure.measure(this.options);

    // Setup loop that draws to screen
    this.refresh(0, this.rows - 1);

    // Initialize global actions that need to be taken on the document.
    this._initGlobal();

    // Listen for mouse events and translate
    // them into terminal mouse protocols.
    this.bindMouse();

  }

  private _createRenderer(): IRenderer {
    switch (this.options.rendererType) {
      case 'canvas': return new Renderer(this, this._colorManager.colors); break;
      case 'dom': return new DomRenderer(this, this._colorManager.colors); break;
      default: throw new Error(`Unrecognized rendererType "${this.options.rendererType}"`);
    }
  }

  /**
   * Sets the theme on the renderer. The renderer must have been initialized.
   * @param theme The theme to set.
   */
  private _setTheme(theme: ITheme): void {
    this._theme = theme;
    this._colorManager.setTheme(theme);
    if (this._renderCoordinator) {
      this._renderCoordinator.setColors(this._colorManager.colors);
    }
    if (this.viewport) {
      this.viewport.onThemeChange(this._colorManager.colors);
    }
  }

  /**
   * XTerm mouse events
   * http://invisible-island.net/xterm/ctlseqs/ctlseqs.html#Mouse%20Tracking
   * To better understand these
   * the xterm code is very helpful:
   * Relevant files:
   *   button.c, charproc.c, misc.c
   * Relevant functions in xterm/button.c:
   *   BtnCode, EmitButtonCode, EditorButton, SendMousePosition
   */
  public bindMouse(): void {
    const el = this.element;
    const self = this;
    let pressed = 32;

    // mouseup, mousedown, wheel
    // left click: ^[[M 3<^[[M#3<
    // wheel up: ^[[M`3>
    function sendButton(ev: MouseEvent | WheelEvent): void {
      let button;
      let pos;

      // get the xterm-style button
      button = getButton(ev);

      // get mouse coordinates
      pos = self.mouseHelper.getRawByteCoords(ev, self.screenElement, self.charMeasure, self.cols, self.rows);
      if (!pos) return;

      sendEvent(button, pos);

      switch ((<any>ev).overrideType || ev.type) {
        case 'mousedown':
          pressed = button;
          break;
        case 'mouseup':
          // keep it at the left
          // button, just in case.
          pressed = 32;
          break;
        case 'wheel':
          // nothing. don't
          // interfere with
          // `pressed`.
          break;
      }
    }

    // motion example of a left click:
    // ^[[M 3<^[[M@4<^[[M@5<^[[M@6<^[[M@7<^[[M#7<
    function sendMove(ev: MouseEvent): void {
      let button = pressed;
      const pos = self.mouseHelper.getRawByteCoords(ev, self.screenElement, self.charMeasure, self.cols, self.rows);
      if (!pos) return;

      // buttons marked as motions
      // are incremented by 32
      button += 32;

      sendEvent(button, pos);
    }

    // encode button and
    // position to characters
    function encode(data: number[], ch: number): void {
      if (!self.utfMouse) {
        if (ch === 255) {
          data.push(0);
          return;
        }
        if (ch > 127) ch = 127;
        data.push(ch);
      } else {
        if (ch > 2047) {
          data.push(2047);
          return;
        }
        data.push(ch);
      }
    }

    // send a mouse event:
    // regular/utf8: ^[[M Cb Cx Cy
    // urxvt: ^[[ Cb ; Cx ; Cy M
    // sgr: ^[[ Cb ; Cx ; Cy M/m
    // vt300: ^[[ 24(1/3/5)~ [ Cx , Cy ] \r
    // locator: CSI P e ; P b ; P r ; P c ; P p & w
    function sendEvent(button: number, pos: {x: number, y: number}): void {
      // self.emit('mouse', {
      //   x: pos.x - 32,
      //   y: pos.x - 32,
      //   button: button
      // });

      if (self._vt300Mouse) {
        // NOTE: Unstable.
        // http://www.vt100.net/docs/vt3xx-gp/chapter15.html
        button &= 3;
        pos.x -= 32;
        pos.y -= 32;
        let data = C0.ESC + '[24';
        if (button === 0) data += '1';
        else if (button === 1) data += '3';
        else if (button === 2) data += '5';
        else if (button === 3) return;
        else data += '0';
        data += '~[' + pos.x + ',' + pos.y + ']\r';
        self.handler(data);
        return;
      }

      if (self._decLocator) {
        // NOTE: Unstable.
        button &= 3;
        pos.x -= 32;
        pos.y -= 32;
        if (button === 0) button = 2;
        else if (button === 1) button = 4;
        else if (button === 2) button = 6;
        else if (button === 3) button = 3;
        self.handler(C0.ESC + '['
                  + button
                  + ';'
                  + (button === 3 ? 4 : 0)
                  + ';'
                  + pos.y
                  + ';'
                  + pos.x
                  + ';'
                  // Not sure what page is meant to be
                  + (<any>pos).page || 0
                  + '&w');
        return;
      }

      if (self.urxvtMouse) {
        pos.x -= 32;
        pos.y -= 32;
        pos.x++;
        pos.y++;
        self.handler(C0.ESC + '[' + button + ';' + pos.x + ';' + pos.y + 'M');
        return;
      }

      if (self.sgrMouse) {
        pos.x -= 32;
        pos.y -= 32;
        self.handler(C0.ESC + '[<'
                  + (((button & 3) === 3 ? button & ~3 : button) - 32)
                  + ';'
                  + pos.x
                  + ';'
                  + pos.y
                  + ((button & 3) === 3 ? 'm' : 'M'));
        return;
      }

      const data: number[] = [];

      encode(data, button);
      encode(data, pos.x);
      encode(data, pos.y);

      self.handler(C0.ESC + '[M' + String.fromCharCode.apply(String, data));
    }

    function getButton(ev: MouseEvent): number {
      let button;
      let shift;
      let meta;
      let ctrl;
      let mod;

      // two low bits:
      // 0 = left
      // 1 = middle
      // 2 = right
      // 3 = release
      // wheel up/down:
      // 1, and 2 - with 64 added
      switch ((<any>ev).overrideType || ev.type) {
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
          button = (<WheelEvent>ev).deltaY < 0
            ? 64
          : 65;
          break;
      }

      // next three bits are the modifiers:
      // 4 = shift, 8 = meta, 16 = control
      shift = ev.shiftKey ? 4 : 0;
      meta = ev.metaKey ? 8 : 0;
      ctrl = ev.ctrlKey ? 16 : 0;
      mod = shift | meta | ctrl;

      // no mods
      if (self.vt200Mouse) {
        // ctrl only
        mod &= ctrl;
      } else if (!self.normalMouse) {
        mod = 0;
      }

      // increment to SP
      button = (32 + (mod << 2)) + button;

      return button;
    }

    this.register(addDisposableDomListener(el, 'mousedown', (ev: MouseEvent) => {

      // Prevent the focus on the textarea from getting lost
      // and make sure we get focused on mousedown
      ev.preventDefault();
      this.focus();

      // Don't send the mouse button to the pty if mouse events are disabled or
      // if the selection manager is having selection forced (ie. a modifier is
      // held).
      if (!this.mouseEvents || this.selectionManager.shouldForceSelection(ev)) {
        return;
      }

      // send the button
      sendButton(ev);

      // fix for odd bug
      // if (this.vt200Mouse && !this.normalMouse) {
      if (this.vt200Mouse) {
        (<any>ev).overrideType = 'mouseup';
        sendButton(ev);
        return this.cancel(ev);
      }

      // TODO: All mouse handling should be pulled into its own file.

      // bind events
      let moveHandler: (event: MouseEvent) => void;
      if (this.normalMouse) {
        moveHandler = (event: MouseEvent) => {
          // Do nothing if normal mouse mode is on. This can happen if the mouse is held down when the
          // terminal exits normalMouse mode.
          if (!this.normalMouse) {
            return;
          }
          sendMove(event);
        };
        // TODO: these event listeners should be managed by the disposable, the Terminal reference may
        // be kept aroud if Terminal.dispose is fired when the mouse is down
        this._document.addEventListener('mousemove', moveHandler);
      }

      // x10 compatibility mode can't send button releases
      const handler = (ev: MouseEvent) => {
        if (this.normalMouse && !this.x10Mouse) {
          sendButton(ev);
        }
        if (moveHandler) {
          // Even though this should only be attached when this.normalMouse is true, holding the
          // mouse button down when normalMouse changes can happen. Just always try to remove it.
          this._document.removeEventListener('mousemove', moveHandler);
          moveHandler = null;
        }
        this._document.removeEventListener('mouseup', handler);
        return this.cancel(ev);
      };
      this._document.addEventListener('mouseup', handler);

      return this.cancel(ev);
    }));

    // if (this.normalMouse) {
    //  on(this.document, 'mousemove', sendMove);
    // }

    this.register(addDisposableDomListener(el, 'wheel', (ev: WheelEvent) => {
      if (!this.mouseEvents) {
        // Convert wheel events into up/down events when the buffer does not have scrollback, this
        // enables scrolling in apps hosted in the alt buffer such as vim or tmux.
        if (!this.buffer.hasScrollback) {
          const amount = this.viewport.getLinesScrolled(ev);

          // Do nothing if there's no vertical scroll
          if (amount === 0) {
            return;
          }

          // Construct and send sequences
          const sequence = C0.ESC + (this.applicationCursor ? 'O' : '[') + ( ev.deltaY < 0 ? 'A' : 'B');
          let data = '';
          for (let i = 0; i < Math.abs(amount); i++) {
            data += sequence;
          }
          this.handler(data);
        }
        return;
      }
      if (this.x10Mouse || this._vt300Mouse || this._decLocator) return;
      sendButton(ev);
      ev.preventDefault();
    }));

    // allow wheel scrolling in
    // the shell for example
    this.register(addDisposableDomListener(el, 'wheel', (ev: WheelEvent) => {
      if (this.mouseEvents) return;
      this.viewport.onWheel(ev);
      return this.cancel(ev);
    }));

    this.register(addDisposableDomListener(el, 'touchstart', (ev: TouchEvent) => {
      if (this.mouseEvents) return;
      this.viewport.onTouchStart(ev);
      return this.cancel(ev);
    }));

    this.register(addDisposableDomListener(el, 'touchmove', (ev: TouchEvent) => {
      if (this.mouseEvents) return;
      this.viewport.onTouchMove(ev);
      return this.cancel(ev);
    }));
  }

  /**
   * Tells the renderer to refresh terminal content between two rows (inclusive) at the next
   * opportunity.
   * @param start The row to start from (between 0 and this.rows - 1).
   * @param end The row to end at (between start and this.rows - 1).
   */
  public refresh(start: number, end: number): void {
    if (this._renderCoordinator) {
      this._renderCoordinator.refreshRows(start, end);
    }
  }

  /**
   * Queues linkification for the specified rows.
   * @param start The row to start from (between 0 and this.rows - 1).
   * @param end The row to end at (between start and this.rows - 1).
   */
  private _queueLinkification(start: number, end: number): void {
    if (this.linkifier) {
      this.linkifier.linkifyRows(start, end);
    }
  }

  /**
   * Change the cursor style for different selection modes
   */
  public updateCursorStyle(ev: KeyboardEvent): void {
    if (this.selectionManager && this.selectionManager.shouldColumnSelect(ev)) {
      this.element.classList.add('column-select');
    } else {
      this.element.classList.remove('column-select');
    }
  }

  /**
   * Display the cursor element
   */
  public showCursor(): void {
    if (!this.cursorState) {
      this.cursorState = 1;
      this.refresh(this.buffer.y, this.buffer.y);
    }
  }

  /**
   * Scroll the terminal down 1 row, creating a blank line.
   * @param isWrapped Whether the new line is wrapped from the previous line.
   */
  public scroll(isWrapped: boolean = false): void {
    let newLine: IBufferLine;
    newLine = this._blankLine;
    const eraseAttr = this.eraseAttrData();
    if (!newLine || newLine.length !== this.cols || newLine.getFg(0) !== eraseAttr.fg || newLine.getBg(0) !== eraseAttr.bg) {
      newLine = this.buffer.getBlankLine(eraseAttr, isWrapped);
      this._blankLine = newLine;
    }
    newLine.isWrapped = isWrapped;

    const topRow = this.buffer.ybase + this.buffer.scrollTop;
    const bottomRow = this.buffer.ybase + this.buffer.scrollBottom;

    if (this.buffer.scrollTop === 0) {
      // Determine whether the buffer is going to be trimmed after insertion.
      const willBufferBeTrimmed = this.buffer.lines.isFull;

      // Insert the line using the fastest method
      if (bottomRow === this.buffer.lines.length - 1) {
        if (willBufferBeTrimmed) {
          this.buffer.lines.recycle().copyFrom(newLine);
        } else {
          this.buffer.lines.push(newLine.clone());
        }
      } else {
        this.buffer.lines.splice(bottomRow + 1, 0, newLine.clone());
      }

      // Only adjust ybase and ydisp when the buffer is not trimmed
      if (!willBufferBeTrimmed) {
        this.buffer.ybase++;
        // Only scroll the ydisp with ybase if the user has not scrolled up
        if (!this._userScrolling) {
          this.buffer.ydisp++;
        }
      } else {
        // When the buffer is full and the user has scrolled up, keep the text
        // stable unless ydisp is right at the top
        if (this._userScrolling) {
          this.buffer.ydisp = Math.max(this.buffer.ydisp - 1, 0);
        }
      }
    } else {
      // scrollTop is non-zero which means no line will be going to the
      // scrollback, instead we can just shift them in-place.
      const scrollRegionHeight = bottomRow - topRow + 1/*as it's zero-based*/;
      this.buffer.lines.shiftElements(topRow + 1, scrollRegionHeight - 1, -1);
      this.buffer.lines.set(bottomRow, newLine.clone());
    }

    // Move the viewport to the bottom of the buffer unless the user is
    // scrolling.
    if (!this._userScrolling) {
      this.buffer.ydisp = this.buffer.ybase;
    }

    // Flag rows that need updating
    this.updateRange(this.buffer.scrollTop);
    this.updateRange(this.buffer.scrollBottom);

    this._onScroll.fire(this.buffer.ydisp);
  }

  /**
   * Scroll the display of the terminal
   * @param disp The number of lines to scroll down (negative scroll up).
   * @param suppressScrollEvent Don't emit the scroll event as scrollLines. This is used
   * to avoid unwanted events being handled by the viewport when the event was triggered from the
   * viewport originally.
   */
  public scrollLines(disp: number, suppressScrollEvent?: boolean): void {
    if (disp < 0) {
      if (this.buffer.ydisp === 0) {
        return;
      }
      this._userScrolling = true;
    } else if (disp + this.buffer.ydisp >= this.buffer.ybase) {
      this._userScrolling = false;
    }

    const oldYdisp = this.buffer.ydisp;
    this.buffer.ydisp = Math.max(Math.min(this.buffer.ydisp + disp, this.buffer.ybase), 0);

    // No change occurred, don't trigger scroll/refresh
    if (oldYdisp === this.buffer.ydisp) {
      return;
    }

    if (!suppressScrollEvent) {
      this._onScroll.fire(this.buffer.ydisp);
    }

    this.refresh(0, this.rows - 1);
  }

  /**
   * Scroll the display of the terminal by a number of pages.
   * @param pageCount The number of pages to scroll (negative scrolls up).
   */
  public scrollPages(pageCount: number): void {
    this.scrollLines(pageCount * (this.rows - 1));
  }

  /**
   * Scrolls the display of the terminal to the top.
   */
  public scrollToTop(): void {
    this.scrollLines(-this.buffer.ydisp);
  }

  /**
   * Scrolls the display of the terminal to the bottom.
   */
  public scrollToBottom(): void {
    this.scrollLines(this.buffer.ybase - this.buffer.ydisp);
  }

  public scrollToLine(line: number): void {
    const scrollAmount = line - this.buffer.ydisp;
    if (scrollAmount !== 0) {
      this.scrollLines(scrollAmount);
    }
  }

  /**
   * Writes raw utf8 bytes to the terminal.
   * @param data UintArray with UTF8 bytes to write to the terminal.
   */
  public writeUtf8(data: Uint8Array): void {
    // Ensure the terminal isn't disposed
    if (this._isDisposed) {
      return;
    }

    // Ignore falsy data values
    if (!data) {
      return;
    }

    this.writeBufferUtf8.push(data);

    // Send XOFF to pause the pty process if the write buffer becomes too large so
    // xterm.js can catch up before more data is sent. This is necessary in order
    // to keep signals such as ^C responsive.
    if (this.options.useFlowControl && !this._xoffSentToCatchUp && this.writeBufferUtf8.length >= WRITE_BUFFER_PAUSE_THRESHOLD) {
      // XOFF - stop pty pipe
      // XON will be triggered by emulator before processing data chunk
      this.handler(C0.DC3);
      this._xoffSentToCatchUp = true;
    }

    if (!this._writeInProgress && this.writeBufferUtf8.length > 0) {
      // Kick off a write which will write all data in sequence recursively
      this._writeInProgress = true;
      // Kick off an async innerWrite so more writes can come in while processing data
      setTimeout(() => {
        this._innerWriteUtf8();
      });
    }
  }

  protected _innerWriteUtf8(bufferOffset: number = 0): void {
    // Ensure the terminal isn't disposed
    if (this._isDisposed) {
      this.writeBufferUtf8 = [];
    }

    const startTime = Date.now();
    while (this.writeBufferUtf8.length > bufferOffset) {
      const data = this.writeBufferUtf8[bufferOffset];
      bufferOffset++;

      // If XOFF was sent in order to catch up with the pty process, resume it if
      // we reached the end of the writeBuffer to allow more data to come in.
      if (this._xoffSentToCatchUp && this.writeBufferUtf8.length === bufferOffset) {
        this.handler(C0.DC1);
        this._xoffSentToCatchUp = false;
      }

      this._refreshStart = this.buffer.y;
      this._refreshEnd = this.buffer.y;

      // HACK: Set the parser state based on it's state at the time of return.
      // This works around the bug #662 which saw the parser state reset in the
      // middle of parsing escape sequence in two chunks. For some reason the
      // state of the parser resets to 0 after exiting parser.parse. This change
      // just sets the state back based on the correct return statement.

      this._inputHandler.parseUtf8(data);

      this.updateRange(this.buffer.y);
      this.refresh(this._refreshStart, this._refreshEnd);

      if (Date.now() - startTime >= WRITE_TIMEOUT_MS) {
        break;
      }
    }
    if (this.writeBufferUtf8.length > bufferOffset) {
      // Allow renderer to catch up before processing the next batch
      // trim already processed chunks if we are above threshold
      if (bufferOffset > WRITE_BUFFER_LENGTH_THRESHOLD) {
        this.writeBufferUtf8 = this.writeBufferUtf8.slice(bufferOffset);
        bufferOffset = 0;
      }
      setTimeout(() => this._innerWriteUtf8(bufferOffset), 0);
    } else {
      this._writeInProgress = false;
      this.writeBufferUtf8 = [];
    }
  }

  /**
   * Writes text to the terminal.
   * @param data The text to write to the terminal.
   */
  public write(data: string): void {
    // Ensure the terminal isn't disposed
    if (this._isDisposed) {
      return;
    }

    // Ignore falsy data values (including the empty string)
    if (!data) {
      return;
    }

    this.writeBuffer.push(data);

    // Send XOFF to pause the pty process if the write buffer becomes too large so
    // xterm.js can catch up before more data is sent. This is necessary in order
    // to keep signals such as ^C responsive.
    if (this.options.useFlowControl && !this._xoffSentToCatchUp && this.writeBuffer.length >= WRITE_BUFFER_PAUSE_THRESHOLD) {
      // XOFF - stop pty pipe
      // XON will be triggered by emulator before processing data chunk
      this.handler(C0.DC3);
      this._xoffSentToCatchUp = true;
    }

    if (!this._writeInProgress && this.writeBuffer.length > 0) {
      // Kick off a write which will write all data in sequence recursively
      this._writeInProgress = true;
      // Kick off an async innerWrite so more writes can come in while processing data
      setTimeout(() => {
        this._innerWrite();
      });
    }
  }

  protected _innerWrite(bufferOffset: number = 0): void {
    // Ensure the terminal isn't disposed
    if (this._isDisposed) {
      this.writeBuffer = [];
    }

    const startTime = Date.now();
    while (this.writeBuffer.length > bufferOffset) {
      const data = this.writeBuffer[bufferOffset];
      bufferOffset++;

      // If XOFF was sent in order to catch up with the pty process, resume it if
      // we reached the end of the writeBuffer to allow more data to come in.
      if (this._xoffSentToCatchUp && this.writeBuffer.length === bufferOffset) {
        this.handler(C0.DC1);
        this._xoffSentToCatchUp = false;
      }

      this._refreshStart = this.buffer.y;
      this._refreshEnd = this.buffer.y;

      // HACK: Set the parser state based on it's state at the time of return.
      // This works around the bug #662 which saw the parser state reset in the
      // middle of parsing escape sequence in two chunks. For some reason the
      // state of the parser resets to 0 after exiting parser.parse. This change
      // just sets the state back based on the correct return statement.

      this._inputHandler.parse(data);

      this.updateRange(this.buffer.y);
      this.refresh(this._refreshStart, this._refreshEnd);

      if (Date.now() - startTime >= WRITE_TIMEOUT_MS) {
        break;
      }
    }
    if (this.writeBuffer.length > bufferOffset) {
      // Allow renderer to catch up before processing the next batch
      // trim already processed chunks if we are above threshold
      if (bufferOffset > WRITE_BUFFER_LENGTH_THRESHOLD) {
        this.writeBuffer = this.writeBuffer.slice(bufferOffset);
        bufferOffset = 0;
      }
      setTimeout(() => this._innerWrite(bufferOffset), 0);
    } else {
      this._writeInProgress = false;
      this.writeBuffer = [];
    }
  }

  /**
   * Writes text to the terminal, followed by a break line character (\n).
   * @param data The text to write to the terminal.
   */
  public writeln(data: string): void {
    this.write(data + '\r\n');
  }

  /**
   * Attaches a custom key event handler which is run before keys are processed,
   * giving consumers of xterm.js ultimate control as to what keys should be
   * processed by the terminal and what keys should not.
   * @param customKeyEventHandler The custom KeyboardEvent handler to attach.
   * This is a function that takes a KeyboardEvent, allowing consumers to stop
   * propagation and/or prevent the default action. The function returns whether
   * the event should be processed by xterm.js.
   */
  public attachCustomKeyEventHandler(customKeyEventHandler: CustomKeyEventHandler): void {
    this._customKeyEventHandler = customKeyEventHandler;
  }

  /** Add handler for CSI escape sequence. See xterm.d.ts for details. */
  public addCsiHandler(flag: string, callback: (params: number[], collect: string) => boolean): IDisposable {
    return this._inputHandler.addCsiHandler(flag, callback);
  }
  /** Add handler for OSC escape sequence. See xterm.d.ts for details. */
  public addOscHandler(ident: number, callback: (data: string) => boolean): IDisposable {
    return this._inputHandler.addOscHandler(ident, callback);
  }

  /**
   * Registers a link matcher, allowing custom link patterns to be matched and
   * handled.
   * @param regex The regular expression to search for, specifically
   * this searches the textContent of the rows. You will want to use \s to match
   * a space ' ' character for example.
   * @param handler The callback when the link is called.
   * @param options Options for the link matcher.
   * @return The ID of the new matcher, this can be used to deregister.
   */
  public registerLinkMatcher(regex: RegExp, handler: LinkMatcherHandler, options?: ILinkMatcherOptions): number {
    const matcherId = this.linkifier.registerLinkMatcher(regex, handler, options);
    this.refresh(0, this.rows - 1);
    return matcherId;
  }

  /**
   * Deregisters a link matcher if it has been registered.
   * @param matcherId The link matcher's ID (returned after register)
   */
  public deregisterLinkMatcher(matcherId: number): void {
    if (this.linkifier.deregisterLinkMatcher(matcherId)) {
      this.refresh(0, this.rows - 1);
    }
  }

  public registerCharacterJoiner(handler: CharacterJoinerHandler): number {
    const joinerId = this._renderCoordinator.registerCharacterJoiner(handler);
    this.refresh(0, this.rows - 1);
    return joinerId;
  }

  public deregisterCharacterJoiner(joinerId: number): void {
    if (this._renderCoordinator.deregisterCharacterJoiner(joinerId)) {
      this.refresh(0, this.rows - 1);
    }
  }

  public get markers(): IMarker[] {
    return this.buffer.markers;
  }

  public addMarker(cursorYOffset: number): IMarker {
    // Disallow markers on the alt buffer
    if (this.buffer !== this.buffers.normal) {
      return;
    }

    return this.buffer.addMarker(this.buffer.ybase + this.buffer.y + cursorYOffset);
  }

  /**
   * Gets whether the terminal has an active selection.
   */
  public hasSelection(): boolean {
    return this.selectionManager ? this.selectionManager.hasSelection : false;
  }

  /**
   * Selects text within the terminal.
   * @param column The column the selection starts at..
   * @param row The row the selection starts at.
   * @param length The length of the selection.
   */
  public select(column: number, row: number, length: number): void {
    this.selectionManager.setSelection(column, row, length);
  }

  /**
   * Gets the terminal's current selection, this is useful for implementing copy
   * behavior outside of xterm.js.
   */
  public getSelection(): string {
    return this.selectionManager ? this.selectionManager.selectionText : '';
  }

  public getSelectionPosition(): ISelectionPosition | undefined {
    if (!this.selectionManager.hasSelection) {
      return undefined;
    }

    return {
      startColumn: this.selectionManager.selectionStart[0],
      startRow: this.selectionManager.selectionStart[1],
      endColumn: this.selectionManager.selectionEnd[0],
      endRow: this.selectionManager.selectionEnd[1]
    };
  }

  /**
   * Clears the current terminal selection.
   */
  public clearSelection(): void {
    if (this.selectionManager) {
      this.selectionManager.clearSelection();
    }
  }

  /**
   * Selects all text within the terminal.
   */
  public selectAll(): void {
    if (this.selectionManager) {
      this.selectionManager.selectAll();
    }
  }

  public selectLines(start: number, end: number): void {
    if (this.selectionManager) {
      this.selectionManager.selectLines(start, end);
    }
  }

  /**
   * Handle a keydown event
   * Key Resources:
   *   - https://developer.mozilla.org/en-US/docs/DOM/KeyboardEvent
   * @param ev The keydown event to be handled.
   */
  protected _keyDown(event: KeyboardEvent): boolean {
    if (this._customKeyEventHandler && this._customKeyEventHandler(event) === false) {
      return false;
    }

    if (!this._compositionHelper.keydown(event)) {
      if (this.buffer.ybase !== this.buffer.ydisp) {
        this.scrollToBottom();
      }
      return false;
    }

    const result = evaluateKeyboardEvent(event, this.applicationCursor, this.browser.isMac, this.options.macOptionIsMeta);

    this.updateCursorStyle(event);

    // if (result.key === C0.DC3) { // XOFF
    //   this._writeStopped = true;
    // } else if (result.key === C0.DC1) { // XON
    //   this._writeStopped = false;
    // }

    if (result.type === KeyboardResultType.PAGE_DOWN || result.type === KeyboardResultType.PAGE_UP) {
      const scrollCount = this.rows - 1;
      this.scrollLines(result.type === KeyboardResultType.PAGE_UP ? -scrollCount : scrollCount);
      return this.cancel(event, true);
    }

    if (result.type === KeyboardResultType.SELECT_ALL) {
      this.selectAll();
    }

    if (this._isThirdLevelShift(this.browser, event)) {
      return true;
    }

    if (result.cancel) {
      // The event is canceled at the end already, is this necessary?
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
  }

  private _isThirdLevelShift(browser: IBrowser, ev: IKeyboardEvent): boolean {
    const thirdLevelKey =
        (browser.isMac && !this.options.macOptionIsMeta && ev.altKey && !ev.ctrlKey && !ev.metaKey) ||
        (browser.isMSWindows && ev.altKey && ev.ctrlKey && !ev.metaKey);

    if (ev.type === 'keypress') {
      return thirdLevelKey;
    }

    // Don't invoke for arrows, pageDown, home, backspace, etc. (on non-keypress events)
    return thirdLevelKey && (!ev.keyCode || ev.keyCode > 47);
  }

  /**
   * Set the G level of the terminal
   * @param g
   */
  public setgLevel(g: number): void {
    this.glevel = g;
    this.charset = this.charsets[g];
  }

  /**
   * Set the charset for the given G level of the terminal
   * @param g
   * @param charset
   */
  public setgCharset(g: number, charset: ICharset): void {
    this.charsets[g] = charset;
    if (this.glevel === g) {
      this.charset = charset;
    }
  }

  protected _keyUp(ev: KeyboardEvent): void {
    this.updateCursorStyle(ev);
  }

  /**
   * Handle a keypress event.
   * Key Resources:
   *   - https://developer.mozilla.org/en-US/docs/DOM/KeyboardEvent
   * @param ev The keypress event to be handled.
   */
  protected _keyPress(ev: KeyboardEvent): boolean {
    let key;

    if (this._customKeyEventHandler && this._customKeyEventHandler(ev) === false) {
      return false;
    }

    this.cancel(ev);

    if (ev.charCode) {
      key = ev.charCode;
    } else if (ev.which === null || ev.which === undefined) {
      key = ev.keyCode;
    } else if (ev.which !== 0 && ev.charCode !== 0) {
      key = ev.which;
    } else {
      return false;
    }

    if (!key || (
      (ev.altKey || ev.ctrlKey || ev.metaKey) && !this._isThirdLevelShift(this.browser, ev)
    )) {
      return false;
    }

    key = String.fromCharCode(key);

    this.emit('keypress', key, ev);
    this._onKey.fire({ key, domEvent: ev });
    this.showCursor();
    this.handler(key);

    return true;
  }

  /**
   * Ring the bell.
   * Note: We could do sweet things with webaudio here
   */
  public bell(): void {
    this.emit('bell');
    if (this._soundBell()) {
      this.soundManager.playBellSound();
    }

    if (this._visualBell()) {
      this.element.classList.add('visual-bell-active');
      clearTimeout(this._visualBellTimer);
      this._visualBellTimer = window.setTimeout(() => {
        this.element.classList.remove('visual-bell-active');
      }, 200);
    }
  }

  /**
   * Log the current state to the console.
   */
  public log(text: string, data?: any): void {
    if (!this.options.debug) return;
    if (!this._context.console || !this._context.console.log) return;
    this._context.console.log(text, data);
  }

  /**
   * Log the current state as error to the console.
   */
  public error(text: string, data?: any): void {
    if (!this.options.debug) return;
    if (!this._context.console || !this._context.console.error) return;
    this._context.console.error(text, data);
  }

  /**
   * Resizes the terminal.
   *
   * @param x The number of columns to resize to.
   * @param y The number of rows to resize to.
   */
  public resize(x: number, y: number): void {
    if (isNaN(x) || isNaN(y)) {
      return;
    }

    if (x === this.cols && y === this.rows) {
      // Check if we still need to measure the char size (fixes #785).
      if (this.charMeasure && (!this.charMeasure.width || !this.charMeasure.height)) {
        this.charMeasure.measure(this.options);
      }
      return;
    }

    if (x < MINIMUM_COLS) x = MINIMUM_COLS;
    if (y < MINIMUM_ROWS) y = MINIMUM_ROWS;

    this.buffers.resize(x, y);

    this.cols = x;
    this.rows = y;
    this.buffers.setupTabStops(this.cols);

    if (this.charMeasure) {
      this.charMeasure.measure(this.options);
    }

    this.refresh(0, this.rows - 1);
    this._onResize.fire({ cols: x, rows: y });
  }

  /**
   * Updates the range of rows to refresh
   * @param y The number of rows to refresh next.
   */
  public updateRange(y: number): void {
    if (y < this._refreshStart) this._refreshStart = y;
    if (y > this._refreshEnd) this._refreshEnd = y;
    // if (y > this.refreshEnd) {
    //   this.refreshEnd = y;
    //   if (y > this.rows - 1) {
    //     this.refreshEnd = this.rows - 1;
    //   }
    // }
  }

  /**
   * Set the range of refreshing to the maximum value
   */
  public maxRange(): void {
    this._refreshStart = 0;
    this._refreshEnd = this.rows - 1;
  }

  /**
   * Clear the entire buffer, making the prompt line the new first line.
   */
  public clear(): void {
    if (this.buffer.ybase === 0 && this.buffer.y === 0) {
      // Don't clear if it's already clear
      return;
    }
    this.buffer.lines.set(0, this.buffer.lines.get(this.buffer.ybase + this.buffer.y));
    this.buffer.lines.length = 1;
    this.buffer.ydisp = 0;
    this.buffer.ybase = 0;
    this.buffer.y = 0;
    for (let i = 1; i < this.rows; i++) {
      this.buffer.lines.push(this.buffer.getBlankLine(DEFAULT_ATTR_DATA));
    }
    this.refresh(0, this.rows - 1);
    this._onScroll.fire(this.buffer.ydisp);
  }

  /**
   * Evaluate if the current terminal is the given argument.
   * @param term The terminal name to evaluate
   */
  public is(term: string): boolean {
    return (this.options.termName + '').indexOf(term) === 0;
  }

  /**
   * Emit the data event and populate the given data.
   * @param data The data to populate in the event.
   */
  public handler(data: string): void {
    // Prevents all events to pty process if stdin is disabled
    if (this.options.disableStdin) {
      return;
    }

    // Clear the selection if the selection manager is available and has an active selection
    if (this.selectionManager && this.selectionManager.hasSelection) {
      this.selectionManager.clearSelection();
    }

    // Input is being sent to the terminal, the terminal should focus the prompt.
    if (this.buffer.ybase !== this.buffer.ydisp) {
      this.scrollToBottom();
    }
    this._onData.fire(data);
  }

  /**
   * Emit the 'title' event and populate the given title.
   * @param title The title to populate in the event.
   */
  public handleTitle(title: string): void {
    this._onTitleChange.fire(title);
  }

  /**
   * ESC
   */

  /**
   * ESC D Index (IND is 0x84).
   */
  public index(): void {
    this.buffer.y++;
    if (this.buffer.y > this.buffer.scrollBottom) {
      this.buffer.y--;
      this.scroll();
    }
    // If the end of the line is hit, prevent this action from wrapping around to the next line.
    if (this.buffer.x >= this.cols) {
      this.buffer.x--;
    }
  }

  /**
   * ESC M Reverse Index (RI is 0x8d).
   *
   * Move the cursor up one row, inserting a new blank line if necessary.
   */
  public reverseIndex(): void {
    if (this.buffer.y === this.buffer.scrollTop) {
      // possibly move the code below to term.reverseScroll();
      // test: echo -ne '\e[1;1H\e[44m\eM\e[0m'
      // blankLine(true) is xterm/linux behavior
      const scrollRegionHeight = this.buffer.scrollBottom - this.buffer.scrollTop;
      this.buffer.lines.shiftElements(this.buffer.y + this.buffer.ybase, scrollRegionHeight, 1);
      this.buffer.lines.set(this.buffer.y + this.buffer.ybase, this.buffer.getBlankLine(this.eraseAttrData()));
      this.updateRange(this.buffer.scrollTop);
      this.updateRange(this.buffer.scrollBottom);
    } else {
      this.buffer.y--;
    }
  }

  /**
   * ESC c Full Reset (RIS).
   */
  public reset(): void {
    /**
     * Since _setup handles a full terminal creation, we have to carry forward
     * a few things that should not reset.
     */
    this.options.rows = this.rows;
    this.options.cols = this.cols;
    const customKeyEventHandler = this._customKeyEventHandler;
    const inputHandler = this._inputHandler;
    const cursorState = this.cursorState;
    const writeBuffer = this.writeBuffer;
    const writeBufferUtf8 = this.writeBufferUtf8;
    const writeInProgress = this._writeInProgress;
    const xoffSentToCatchUp = this._xoffSentToCatchUp;
    const userScrolling = this._userScrolling;

    this._setup();

    // reattach
    this._customKeyEventHandler = customKeyEventHandler;
    this._inputHandler = inputHandler;
    this.cursorState = cursorState;
    this.writeBuffer = writeBuffer;
    this.writeBufferUtf8 = writeBufferUtf8;
    this._writeInProgress = writeInProgress;
    this._xoffSentToCatchUp = xoffSentToCatchUp;
    this._userScrolling = userScrolling;

    // do a full screen refresh
    this.refresh(0, this.rows - 1);
    if (this.viewport) {
      this.viewport.syncScrollArea();
    }
  }


  /**
   * ESC H Tab Set (HTS is 0x88).
   */
  public tabSet(): void {
    this.buffer.tabs[this.buffer.x] = true;
  }

  // TODO: Remove cancel function and cancelEvents option
  public cancel(ev: Event, force?: boolean): boolean {
    if (!this.options.cancelEvents && !force) {
      return;
    }
    ev.preventDefault();
    ev.stopPropagation();
    return false;
  }

  private _visualBell(): boolean {
    return false;
    // return this.options.bellStyle === 'visual' ||
    //     this.options.bellStyle === 'both';
  }

  private _soundBell(): boolean {
    return this.options.bellStyle === 'sound';
    // return this.options.bellStyle === 'sound' ||
    //     this.options.bellStyle === 'both';
  }
}

/**
 * Helpers
 */

function wasModifierKeyOnlyEvent(ev: KeyboardEvent): boolean {
  return ev.keyCode === 16 || // Shift
    ev.keyCode === 17 || // Ctrl
    ev.keyCode === 18; // Alt
}
