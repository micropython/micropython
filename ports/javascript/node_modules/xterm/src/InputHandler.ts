/**
 * Copyright (c) 2014 The xterm.js authors. All rights reserved.
 * Copyright (c) 2012-2013, Christopher Jeffrey (MIT License)
 * @license MIT
 */

import { IInputHandler, IDcsHandler, IEscapeSequenceParser, IInputHandlingTerminal } from './Types';
import { C0, C1 } from './common/data/EscapeSequences';
import { CHARSETS, DEFAULT_CHARSET } from './core/data/Charsets';
import { wcwidth } from './CharWidth';
import { EscapeSequenceParser } from './EscapeSequenceParser';
import { IDisposable } from 'xterm';
import { Disposable } from './common/Lifecycle';
import { concat } from './common/TypedArrayUtils';
import { StringToUtf32, stringFromCodePoint, utf32ToString, Utf8ToUtf32 } from './core/input/TextDecoder';
import { CellData, Attributes, FgFlags, BgFlags, AttributeData, NULL_CELL_WIDTH, NULL_CELL_CODE, DEFAULT_ATTR_DATA } from './core/buffer/BufferLine';
import { EventEmitter2, IEvent } from './common/EventEmitter2';

/**
 * Map collect to glevel. Used in `selectCharset`.
 */
const GLEVEL: {[key: string]: number} = {'(': 0, ')': 1, '*': 2, '+': 3, '-': 1, '.': 2};


/**
 * DCS subparser implementations
 */

/**
 * DCS $ q Pt ST
 *   DECRQSS (https://vt100.net/docs/vt510-rm/DECRQSS.html)
 *   Request Status String (DECRQSS), VT420 and up.
 *   Response: DECRPSS (https://vt100.net/docs/vt510-rm/DECRPSS.html)
 */
class DECRQSS implements IDcsHandler {
  private _data: Uint32Array = new Uint32Array(0);

  constructor(private _terminal: any) { }

  hook(collect: string, params: number[], flag: number): void {
    this._data = new Uint32Array(0);
  }

  put(data: Uint32Array, start: number, end: number): void {
    this._data = concat(this._data, data.subarray(start, end));
  }

  unhook(): void {
    const data = utf32ToString(this._data);
    this._data = new Uint32Array(0);
    switch (data) {
      // valid: DCS 1 $ r Pt ST (xterm)
      case '"q': // DECSCA
        return this._terminal.handler(`${C0.ESC}P1$r0"q${C0.ESC}\\`);
      case '"p': // DECSCL
        return this._terminal.handler(`${C0.ESC}P1$r61"p${C0.ESC}\\`);
      case 'r': // DECSTBM
        const pt = '' + (this._terminal.buffer.scrollTop + 1) +
                ';' + (this._terminal.buffer.scrollBottom + 1) + 'r';
        return this._terminal.handler(`${C0.ESC}P1$r${pt}${C0.ESC}\\`);
      case 'm': // SGR
        // TODO: report real settings instead of 0m
        return this._terminal.handler(`${C0.ESC}P1$r0m${C0.ESC}\\`);
      case ' q': // DECSCUSR
        const STYLES: {[key: string]: number} = {'block': 2, 'underline': 4, 'bar': 6};
        let style = STYLES[this._terminal.getOption('cursorStyle')];
        style -= this._terminal.getOption('cursorBlink');
        return this._terminal.handler(`${C0.ESC}P1$r${style} q${C0.ESC}\\`);
      default:
        // invalid: DCS 0 $ r Pt ST (xterm)
        this._terminal.error('Unknown DCS $q %s', data);
        this._terminal.handler(`${C0.ESC}P0$r${C0.ESC}\\`);
    }
  }
}

/**
 * DCS Ps; Ps| Pt ST
 *   DECUDK (https://vt100.net/docs/vt510-rm/DECUDK.html)
 *   not supported
 */

/**
 * DCS + q Pt ST (xterm)
 *   Request Terminfo String
 *   not implemented
 */

/**
 * DCS + p Pt ST (xterm)
 *   Set Terminfo Data
 *   not supported
 */



/**
 * The terminal's standard implementation of IInputHandler, this handles all
 * input from the Parser.
 *
 * Refer to http://invisible-island.net/xterm/ctlseqs/ctlseqs.html to understand
 * each function's header comment.
 */
export class InputHandler extends Disposable implements IInputHandler {
  private _parseBuffer: Uint32Array = new Uint32Array(4096);
  private _stringDecoder: StringToUtf32 = new StringToUtf32();
  private _utf8Decoder: Utf8ToUtf32 = new Utf8ToUtf32();
  private _workCell: CellData = new CellData();

  private _onCursorMove = new EventEmitter2<void>();
  public get onCursorMove(): IEvent<void> { return this._onCursorMove.event; }
  private _onData = new EventEmitter2<string>();
  public get onData(): IEvent<string> { return this._onData.event; }
  private _onLineFeed = new EventEmitter2<void>();
  public get onLineFeed(): IEvent<void> { return this._onLineFeed.event; }
  private _onScroll = new EventEmitter2<number>();
  public get onScroll(): IEvent<number> { return this._onScroll.event; }

  constructor(
      protected _terminal: IInputHandlingTerminal,
      private _parser: IEscapeSequenceParser = new EscapeSequenceParser())
  {
    super();

    this.register(this._parser);

    /**
     * custom fallback handlers
     */
    this._parser.setCsiHandlerFallback((collect: string, params: number[], flag: number) => {
      this._terminal.error('Unknown CSI code: ', { collect, params, flag: String.fromCharCode(flag) });
    });
    this._parser.setEscHandlerFallback((collect: string, flag: number) => {
      this._terminal.error('Unknown ESC code: ', { collect, flag: String.fromCharCode(flag) });
    });
    this._parser.setExecuteHandlerFallback((code: number) => {
      this._terminal.error('Unknown EXECUTE code: ', { code });
    });
    this._parser.setOscHandlerFallback((identifier: number, data: string) => {
      this._terminal.error('Unknown OSC code: ', { identifier, data });
    });

    /**
     * print handler
     */
    this._parser.setPrintHandler((data, start, end): void => this.print(data, start, end));

    /**
     * CSI handler
     */
    this._parser.setCsiHandler('@', (params, collect) => this.insertChars(params));
    this._parser.setCsiHandler('A', (params, collect) => this.cursorUp(params));
    this._parser.setCsiHandler('B', (params, collect) => this.cursorDown(params));
    this._parser.setCsiHandler('C', (params, collect) => this.cursorForward(params));
    this._parser.setCsiHandler('D', (params, collect) => this.cursorBackward(params));
    this._parser.setCsiHandler('E', (params, collect) => this.cursorNextLine(params));
    this._parser.setCsiHandler('F', (params, collect) => this.cursorPrecedingLine(params));
    this._parser.setCsiHandler('G', (params, collect) => this.cursorCharAbsolute(params));
    this._parser.setCsiHandler('H', (params, collect) => this.cursorPosition(params));
    this._parser.setCsiHandler('I', (params, collect) => this.cursorForwardTab(params));
    this._parser.setCsiHandler('J', (params, collect) => this.eraseInDisplay(params));
    this._parser.setCsiHandler('K', (params, collect) => this.eraseInLine(params));
    this._parser.setCsiHandler('L', (params, collect) => this.insertLines(params));
    this._parser.setCsiHandler('M', (params, collect) => this.deleteLines(params));
    this._parser.setCsiHandler('P', (params, collect) => this.deleteChars(params));
    this._parser.setCsiHandler('S', (params, collect) => this.scrollUp(params));
    this._parser.setCsiHandler('T', (params, collect) => this.scrollDown(params, collect));
    this._parser.setCsiHandler('X', (params, collect) => this.eraseChars(params));
    this._parser.setCsiHandler('Z', (params, collect) => this.cursorBackwardTab(params));
    this._parser.setCsiHandler('`', (params, collect) => this.charPosAbsolute(params));
    this._parser.setCsiHandler('a', (params, collect) => this.hPositionRelative(params));
    this._parser.setCsiHandler('b', (params, collect) => this.repeatPrecedingCharacter(params));
    this._parser.setCsiHandler('c', (params, collect) => this.sendDeviceAttributes(params, collect));
    this._parser.setCsiHandler('d', (params, collect) => this.linePosAbsolute(params));
    this._parser.setCsiHandler('e', (params, collect) => this.vPositionRelative(params));
    this._parser.setCsiHandler('f', (params, collect) => this.hVPosition(params));
    this._parser.setCsiHandler('g', (params, collect) => this.tabClear(params));
    this._parser.setCsiHandler('h', (params, collect) => this.setMode(params, collect));
    this._parser.setCsiHandler('l', (params, collect) => this.resetMode(params, collect));
    this._parser.setCsiHandler('m', (params, collect) => this.charAttributes(params));
    this._parser.setCsiHandler('n', (params, collect) => this.deviceStatus(params, collect));
    this._parser.setCsiHandler('p', (params, collect) => this.softReset(params, collect));
    this._parser.setCsiHandler('q', (params, collect) => this.setCursorStyle(params, collect));
    this._parser.setCsiHandler('r', (params, collect) => this.setScrollRegion(params, collect));
    this._parser.setCsiHandler('s', (params, collect) => this.saveCursor(params));
    this._parser.setCsiHandler('u', (params, collect) => this.restoreCursor(params));

    /**
     * execute handler
     */
    this._parser.setExecuteHandler(C0.BEL, () => this.bell());
    this._parser.setExecuteHandler(C0.LF, () => this.lineFeed());
    this._parser.setExecuteHandler(C0.VT, () => this.lineFeed());
    this._parser.setExecuteHandler(C0.FF, () => this.lineFeed());
    this._parser.setExecuteHandler(C0.CR, () => this.carriageReturn());
    this._parser.setExecuteHandler(C0.BS, () => this.backspace());
    this._parser.setExecuteHandler(C0.HT, () => this.tab());
    this._parser.setExecuteHandler(C0.SO, () => this.shiftOut());
    this._parser.setExecuteHandler(C0.SI, () => this.shiftIn());
    // FIXME:   What do to with missing? Old code just added those to print.

    // some C1 control codes - FIXME: should those be enabled by default?
    this._parser.setExecuteHandler(C1.IND, () => this.index());
    this._parser.setExecuteHandler(C1.NEL, () => this.nextLine());
    this._parser.setExecuteHandler(C1.HTS, () => this.tabSet());

    /**
     * OSC handler
     */
    //   0 - icon name + title
    this._parser.setOscHandler(0, (data) => this.setTitle(data));
    //   1 - icon name
    //   2 - title
    this._parser.setOscHandler(2, (data) => this.setTitle(data));
    //   3 - set property X in the form "prop=value"
    //   4 - Change Color Number
    //   5 - Change Special Color Number
    //   6 - Enable/disable Special Color Number c
    //   7 - current directory? (not in xterm spec, see https://gitlab.com/gnachman/iterm2/issues/3939)
    //  10 - Change VT100 text foreground color to Pt.
    //  11 - Change VT100 text background color to Pt.
    //  12 - Change text cursor color to Pt.
    //  13 - Change mouse foreground color to Pt.
    //  14 - Change mouse background color to Pt.
    //  15 - Change Tektronix foreground color to Pt.
    //  16 - Change Tektronix background color to Pt.
    //  17 - Change highlight background color to Pt.
    //  18 - Change Tektronix cursor color to Pt.
    //  19 - Change highlight foreground color to Pt.
    //  46 - Change Log File to Pt.
    //  50 - Set Font to Pt.
    //  51 - reserved for Emacs shell.
    //  52 - Manipulate Selection Data.
    // 104 ; c - Reset Color Number c.
    // 105 ; c - Reset Special Color Number c.
    // 106 ; c; f - Enable/disable Special Color Number c.
    // 110 - Reset VT100 text foreground color.
    // 111 - Reset VT100 text background color.
    // 112 - Reset text cursor color.
    // 113 - Reset mouse foreground color.
    // 114 - Reset mouse background color.
    // 115 - Reset Tektronix foreground color.
    // 116 - Reset Tektronix background color.
    // 117 - Reset highlight color.
    // 118 - Reset Tektronix cursor color.
    // 119 - Reset highlight foreground color.

    /**
     * ESC handlers
     */
    this._parser.setEscHandler('7', () => this.saveCursor([]));
    this._parser.setEscHandler('8', () => this.restoreCursor([]));
    this._parser.setEscHandler('D', () => this.index());
    this._parser.setEscHandler('E', () => this.nextLine());
    this._parser.setEscHandler('H', () => this.tabSet());
    this._parser.setEscHandler('M', () => this.reverseIndex());
    this._parser.setEscHandler('=', () => this.keypadApplicationMode());
    this._parser.setEscHandler('>', () => this.keypadNumericMode());
    this._parser.setEscHandler('c', () => this.reset());
    this._parser.setEscHandler('n', () => this.setgLevel(2));
    this._parser.setEscHandler('o', () => this.setgLevel(3));
    this._parser.setEscHandler('|', () => this.setgLevel(3));
    this._parser.setEscHandler('}', () => this.setgLevel(2));
    this._parser.setEscHandler('~', () => this.setgLevel(1));
    this._parser.setEscHandler('%@', () => this.selectDefaultCharset());
    this._parser.setEscHandler('%G', () => this.selectDefaultCharset());
    for (const flag in CHARSETS) {
      this._parser.setEscHandler('(' + flag, () => this.selectCharset('(' + flag));
      this._parser.setEscHandler(')' + flag, () => this.selectCharset(')' + flag));
      this._parser.setEscHandler('*' + flag, () => this.selectCharset('*' + flag));
      this._parser.setEscHandler('+' + flag, () => this.selectCharset('+' + flag));
      this._parser.setEscHandler('-' + flag, () => this.selectCharset('-' + flag));
      this._parser.setEscHandler('.' + flag, () => this.selectCharset('.' + flag));
      this._parser.setEscHandler('/' + flag, () => this.selectCharset('/' + flag)); // TODO: supported?
    }

    /**
     * error handler
     */
    this._parser.setErrorHandler((state) => {
      this._terminal.error('Parsing error: ', state);
      return state;
    });

    /**
     * DCS handler
     */
    this._parser.setDcsHandler('$q', new DECRQSS(this._terminal));
  }

  public dispose(): void {
    super.dispose();
    this._terminal = null;
  }

  public parse(data: string): void {
    // Ensure the terminal is not disposed
    if (!this._terminal) {
      return;
    }

    let buffer = this._terminal.buffer;
    const cursorStartX = buffer.x;
    const cursorStartY = buffer.y;

    // TODO: Consolidate debug/logging #1560
    if ((<any>this._terminal).debug) {
      this._terminal.log('data: ' + data);
    }

    if (this._parseBuffer.length < data.length) {
      this._parseBuffer = new Uint32Array(data.length);
    }
    this._parser.parse(this._parseBuffer, this._stringDecoder.decode(data, this._parseBuffer));

    buffer = this._terminal.buffer;
    if (buffer.x !== cursorStartX || buffer.y !== cursorStartY) {
      this._onCursorMove.fire();
    }
  }

  public parseUtf8(data: Uint8Array): void {
    // Ensure the terminal is not disposed
    if (!this._terminal) {
      return;
    }

    let buffer = this._terminal.buffer;
    const cursorStartX = buffer.x;
    const cursorStartY = buffer.y;

    // TODO: Consolidate debug/logging #1560
    if ((<any>this._terminal).debug) {
      this._terminal.log('data: ' + data);
    }

    if (this._parseBuffer.length < data.length) {
      this._parseBuffer = new Uint32Array(data.length);
    }
    this._parser.parse(this._parseBuffer, this._utf8Decoder.decode(data, this._parseBuffer));

    buffer = this._terminal.buffer;
    if (buffer.x !== cursorStartX || buffer.y !== cursorStartY) {
      this._terminal.emit('cursormove');
    }
  }

  public print(data: Uint32Array, start: number, end: number): void {
    let code: number;
    let chWidth: number;
    const buffer = this._terminal.buffer;
    const charset = this._terminal.charset;
    const screenReaderMode = this._terminal.options.screenReaderMode;
    const cols = this._terminal.cols;
    const wraparoundMode = this._terminal.wraparoundMode;
    const insertMode = this._terminal.insertMode;
    const curAttr = this._terminal.curAttrData;
    let bufferRow = buffer.lines.get(buffer.y + buffer.ybase);

    this._terminal.updateRange(buffer.y);
    for (let pos = start; pos < end; ++pos) {
      code = data[pos];

      // calculate print space
      // expensive call, therefore we save width in line buffer
      chWidth = wcwidth(code);

      // get charset replacement character
      // charset is only defined for ASCII, therefore we only
      // search for an replacement char if code < 127
      if (code < 127 && charset) {
        const ch = charset[String.fromCharCode(code)];
        if (ch) {
          code = ch.charCodeAt(0);
        }
      }

      if (screenReaderMode) {
        this._terminal.emit('a11y.char', stringFromCodePoint(code));
      }

      // insert combining char at last cursor position
      // FIXME: needs handling after cursor jumps
      // buffer.x should never be 0 for a combining char
      // since they always follow a cell consuming char
      // therefore we can test for buffer.x to avoid overflow left
      if (!chWidth && buffer.x) {
        if (!bufferRow.getWidth(buffer.x - 1)) {
          // found empty cell after fullwidth, need to go 2 cells back
          // it is save to step 2 cells back here
          // since an empty cell is only set by fullwidth chars
          bufferRow.addCodepointToCell(buffer.x - 2, code);
        } else {
          bufferRow.addCodepointToCell(buffer.x - 1, code);
        }
        continue;
      }

      // goto next line if ch would overflow
      // TODO: needs a global min terminal width of 2
      // FIXME: additionally ensure chWidth fits into a line
      //   -->  maybe forbid cols<xy at higher level as it would
      //        introduce a bad runtime penalty here
      if (buffer.x + chWidth - 1 >= cols) {
        // autowrap - DECAWM
        // automatically wraps to the beginning of the next line
        if (wraparoundMode) {
          buffer.x = 0;
          buffer.y++;
          if (buffer.y > buffer.scrollBottom) {
            buffer.y--;
            this._terminal.scroll(true);
          } else {
            // The line already exists (eg. the initial viewport), mark it as a
            // wrapped line
            buffer.lines.get(buffer.y).isWrapped = true;
          }
          // row changed, get it again
          bufferRow = buffer.lines.get(buffer.y + buffer.ybase);
        } else {
          if (chWidth === 2) {
            // FIXME: check for xterm behavior
            // What to do here? We got a wide char that does not fit into last cell
            continue;
          }
          // FIXME: Do we have to set buffer.x to cols - 1, if not wrapping?
        }
      }

      // insert mode: move characters to right
      if (insertMode) {
        // right shift cells according to the width
        bufferRow.insertCells(buffer.x, chWidth, buffer.getNullCell(curAttr));
        // test last cell - since the last cell has only room for
        // a halfwidth char any fullwidth shifted there is lost
        // and will be set to empty cell
        if (bufferRow.getWidth(cols - 1) === 2) {
          bufferRow.setCellFromCodePoint(cols - 1, NULL_CELL_CODE, NULL_CELL_WIDTH, curAttr.fg, curAttr.bg);
        }
      }

      // write current char to buffer and advance cursor
      bufferRow.setCellFromCodePoint(buffer.x++, code, chWidth, curAttr.fg, curAttr.bg);

      // fullwidth char - also set next cell to placeholder stub and advance cursor
      // for graphemes bigger than fullwidth we can simply loop to zero
      // we already made sure above, that buffer.x + chWidth will not overflow right
      if (chWidth > 0) {
        while (--chWidth) {
          // other than a regular empty cell a cell following a wide char has no width
          bufferRow.setCellFromCodePoint(buffer.x++, 0, 0, curAttr.fg, curAttr.bg);
        }
      }
    }
    this._terminal.updateRange(buffer.y);
  }

  /**
   * Forward addCsiHandler from parser.
   */
  public addCsiHandler(flag: string, callback: (params: number[], collect: string) => boolean): IDisposable {
    return this._parser.addCsiHandler(flag, callback);
  }

  /**
   * Forward addOscHandler from parser.
   */
  public addOscHandler(ident: number, callback: (data: string) => boolean): IDisposable {
    return this._parser.addOscHandler(ident, callback);
  }

  /**
   * BEL
   * Bell (Ctrl-G).
   */
  public bell(): void {
    this._terminal.bell();
  }

  /**
   * LF
   * Line Feed or New Line (NL).  (LF  is Ctrl-J).
   */
  public lineFeed(): void {
    // make buffer local for faster access
    const buffer = this._terminal.buffer;

    if (this._terminal.options.convertEol) {
      buffer.x = 0;
    }
    buffer.y++;
    if (buffer.y > buffer.scrollBottom) {
      buffer.y--;
      this._terminal.scroll();
    }
    // If the end of the line is hit, prevent this action from wrapping around to the next line.
    if (buffer.x >= this._terminal.cols) {
      buffer.x--;
    }

    this._onLineFeed.fire();
  }

  /**
   * CR
   * Carriage Return (Ctrl-M).
   */
  public carriageReturn(): void {
    this._terminal.buffer.x = 0;
  }

  /**
   * BS
   * Backspace (Ctrl-H).
   */
  public backspace(): void {
    if (this._terminal.buffer.x > 0) {
      this._terminal.buffer.x--;
    }
  }

  /**
   * TAB
   * Horizontal Tab (HT) (Ctrl-I).
   */
  public tab(): void {
    const originalX = this._terminal.buffer.x;
    this._terminal.buffer.x = this._terminal.buffer.nextStop();
    if (this._terminal.options.screenReaderMode) {
      this._terminal.emit('a11y.tab', this._terminal.buffer.x - originalX);
    }
  }

  /**
   * SO
   * Shift Out (Ctrl-N) -> Switch to Alternate Character Set.  This invokes the
   * G1 character set.
   */
  public shiftOut(): void {
    this._terminal.setgLevel(1);
  }

  /**
   * SI
   * Shift In (Ctrl-O) -> Switch to Standard Character Set.  This invokes the G0
   * character set (the default).
   */
  public shiftIn(): void {
    this._terminal.setgLevel(0);
  }

  /**
   * CSI Ps @
   * Insert Ps (Blank) Character(s) (default = 1) (ICH).
   */
  public insertChars(params: number[]): void {
    this._terminal.buffer.lines.get(this._terminal.buffer.y + this._terminal.buffer.ybase).insertCells(
      this._terminal.buffer.x,
      params[0] || 1,
      this._terminal.buffer.getNullCell(this._terminal.eraseAttrData())
    );
    this._terminal.updateRange(this._terminal.buffer.y);
  }

  /**
   * CSI Ps A
   * Cursor Up Ps Times (default = 1) (CUU).
   */
  public cursorUp(params: number[]): void {
    let param = params[0];
    if (param < 1) {
      param = 1;
    }
    this._terminal.buffer.y -= param;
    if (this._terminal.buffer.y < 0) {
      this._terminal.buffer.y = 0;
    }
  }

  /**
   * CSI Ps B
   * Cursor Down Ps Times (default = 1) (CUD).
   */
  public cursorDown(params: number[]): void {
    let param = params[0];
    if (param < 1) {
      param = 1;
    }
    this._terminal.buffer.y += param;
    if (this._terminal.buffer.y >= this._terminal.rows) {
      this._terminal.buffer.y = this._terminal.rows - 1;
    }
    // If the end of the line is hit, prevent this action from wrapping around to the next line.
    if (this._terminal.buffer.x >= this._terminal.cols) {
      this._terminal.buffer.x--;
    }
  }

  /**
   * CSI Ps C
   * Cursor Forward Ps Times (default = 1) (CUF).
   */
  public cursorForward(params: number[]): void {
    let param = params[0];
    if (param < 1) {
      param = 1;
    }
    this._terminal.buffer.x += param;
    if (this._terminal.buffer.x >= this._terminal.cols) {
      this._terminal.buffer.x = this._terminal.cols - 1;
    }
  }

  /**
   * CSI Ps D
   * Cursor Backward Ps Times (default = 1) (CUB).
   */
  public cursorBackward(params: number[]): void {
    let param = params[0];
    if (param < 1) {
      param = 1;
    }
    // If the end of the line is hit, prevent this action from wrapping around to the next line.
    if (this._terminal.buffer.x >= this._terminal.cols) {
      this._terminal.buffer.x--;
    }
    this._terminal.buffer.x -= param;
    if (this._terminal.buffer.x < 0) {
      this._terminal.buffer.x = 0;
    }
  }

  /**
   * CSI Ps E
   * Cursor Next Line Ps Times (default = 1) (CNL).
   * same as CSI Ps B ?
   */
  public cursorNextLine(params: number[]): void {
    let param = params[0];
    if (param < 1) {
      param = 1;
    }
    this._terminal.buffer.y += param;
    if (this._terminal.buffer.y >= this._terminal.rows) {
      this._terminal.buffer.y = this._terminal.rows - 1;
    }
    this._terminal.buffer.x = 0;
  }


  /**
   * CSI Ps F
   * Cursor Preceding Line Ps Times (default = 1) (CNL).
   * reuse CSI Ps A ?
   */
  public cursorPrecedingLine(params: number[]): void {
    let param = params[0];
    if (param < 1) {
      param = 1;
    }
    this._terminal.buffer.y -= param;
    if (this._terminal.buffer.y < 0) {
      this._terminal.buffer.y = 0;
    }
    this._terminal.buffer.x = 0;
  }


  /**
   * CSI Ps G
   * Cursor Character Absolute  [column] (default = [row,1]) (CHA).
   */
  public cursorCharAbsolute(params: number[]): void {
    let param = params[0];
    if (param < 1) {
      param = 1;
    }
    this._terminal.buffer.x = param - 1;
  }

  /**
   * CSI Ps ; Ps H
   * Cursor Position [row;column] (default = [1,1]) (CUP).
   */
  public cursorPosition(params: number[]): void {
    let col: number;
    let row: number = params[0] - 1;

    if (params.length >= 2) {
      col = params[1] - 1;
    } else {
      col = 0;
    }

    if (row < 0) {
      row = 0;
    } else if (row >= this._terminal.rows) {
      row = this._terminal.rows - 1;
    }

    if (col < 0) {
      col = 0;
    } else if (col >= this._terminal.cols) {
      col = this._terminal.cols - 1;
    }

    this._terminal.buffer.x = col;
    this._terminal.buffer.y = row;
  }

  /**
   * CSI Ps I
   *   Cursor Forward Tabulation Ps tab stops (default = 1) (CHT).
   */
  public cursorForwardTab(params: number[]): void {
    let param = params[0] || 1;
    while (param--) {
      this._terminal.buffer.x = this._terminal.buffer.nextStop();
    }
  }

  /**
   * Helper method to erase cells in a terminal row.
   * The cell gets replaced with the eraseChar of the terminal.
   * @param y row index
   * @param start first cell index to be erased
   * @param end   end - 1 is last erased cell
   */
  private _eraseInBufferLine(y: number, start: number, end: number, clearWrap: boolean = false): void {
    const line = this._terminal.buffer.lines.get(this._terminal.buffer.ybase + y);
    line.replaceCells(
      start,
      end,
      this._terminal.buffer.getNullCell(this._terminal.eraseAttrData())
    );
    if (clearWrap) {
      line.isWrapped = false;
    }
  }

  /**
   * Helper method to reset cells in a terminal row.
   * The cell gets replaced with the eraseChar of the terminal and the isWrapped property is set to false.
   * @param y row index
   */
  private _resetBufferLine(y: number): void {
    this._eraseInBufferLine(y, 0, this._terminal.cols, true);
  }

  /**
   * CSI Ps J  Erase in Display (ED).
   *     Ps = 0  -> Erase Below (default).
   *     Ps = 1  -> Erase Above.
   *     Ps = 2  -> Erase All.
   *     Ps = 3  -> Erase Saved Lines (xterm).
   * CSI ? Ps J
   *   Erase in Display (DECSED).
   *     Ps = 0  -> Selective Erase Below (default).
   *     Ps = 1  -> Selective Erase Above.
   *     Ps = 2  -> Selective Erase All.
   */
  public eraseInDisplay(params: number[]): void {
    let j;
    switch (params[0]) {
      case 0:
        j = this._terminal.buffer.y;
        this._terminal.updateRange(j);
        this._eraseInBufferLine(j++, this._terminal.buffer.x, this._terminal.cols, this._terminal.buffer.x === 0);
        for (; j < this._terminal.rows; j++) {
          this._resetBufferLine(j);
        }
        this._terminal.updateRange(j);
        break;
      case 1:
        j = this._terminal.buffer.y;
        this._terminal.updateRange(j);
        // Deleted front part of line and everything before. This line will no longer be wrapped.
        this._eraseInBufferLine(j, 0, this._terminal.buffer.x + 1, true);
        if (this._terminal.buffer.x + 1 >= this._terminal.cols) {
          // Deleted entire previous line. This next line can no longer be wrapped.
          this._terminal.buffer.lines.get(j + 1).isWrapped = false;
        }
        while (j--) {
          this._resetBufferLine(j);
        }
        this._terminal.updateRange(0);
        break;
      case 2:
        j = this._terminal.rows;
        this._terminal.updateRange(j - 1);
        while (j--) {
          this._resetBufferLine(j);
        }
        this._terminal.updateRange(0);
        break;
      case 3:
        // Clear scrollback (everything not in viewport)
        const scrollBackSize = this._terminal.buffer.lines.length - this._terminal.rows;
        if (scrollBackSize > 0) {
          this._terminal.buffer.lines.trimStart(scrollBackSize);
          this._terminal.buffer.ybase = Math.max(this._terminal.buffer.ybase - scrollBackSize, 0);
          this._terminal.buffer.ydisp = Math.max(this._terminal.buffer.ydisp - scrollBackSize, 0);
          // Force a scroll event to refresh viewport
          this._onScroll.fire(0);
        }
        break;
    }
  }

  /**
   * CSI Ps K  Erase in Line (EL).
   *     Ps = 0  -> Erase to Right (default).
   *     Ps = 1  -> Erase to Left.
   *     Ps = 2  -> Erase All.
   * CSI ? Ps K
   *   Erase in Line (DECSEL).
   *     Ps = 0  -> Selective Erase to Right (default).
   *     Ps = 1  -> Selective Erase to Left.
   *     Ps = 2  -> Selective Erase All.
   */
  public eraseInLine(params: number[]): void {
    switch (params[0]) {
      case 0:
        this._eraseInBufferLine(this._terminal.buffer.y, this._terminal.buffer.x, this._terminal.cols);
        break;
      case 1:
        this._eraseInBufferLine(this._terminal.buffer.y, 0, this._terminal.buffer.x + 1);
        break;
      case 2:
        this._eraseInBufferLine(this._terminal.buffer.y, 0, this._terminal.cols);
        break;
    }
    this._terminal.updateRange(this._terminal.buffer.y);
  }

  /**
   * CSI Ps L
   * Insert Ps Line(s) (default = 1) (IL).
   */
  public insertLines(params: number[]): void {
    let param: number = params[0];
    if (param < 1) {
      param = 1;
    }

    // make buffer local for faster access
    const buffer = this._terminal.buffer;

    const row: number = buffer.y + buffer.ybase;

    const scrollBottomRowsOffset = this._terminal.rows - 1 - buffer.scrollBottom;
    const scrollBottomAbsolute = this._terminal.rows - 1 + buffer.ybase - scrollBottomRowsOffset + 1;
    while (param--) {
      // test: echo -e '\e[44m\e[1L\e[0m'
      // blankLine(true) - xterm/linux behavior
      buffer.lines.splice(scrollBottomAbsolute - 1, 1);
      buffer.lines.splice(row, 0, buffer.getBlankLine(this._terminal.eraseAttrData()));
    }

    // this.maxRange();
    this._terminal.updateRange(buffer.y);
    this._terminal.updateRange(buffer.scrollBottom);
  }

  /**
   * CSI Ps M
   * Delete Ps Line(s) (default = 1) (DL).
   */
  public deleteLines(params: number[]): void {
    let param = params[0];
    if (param < 1) {
      param = 1;
    }

    // make buffer local for faster access
    const buffer = this._terminal.buffer;

    const row: number = buffer.y + buffer.ybase;

    let j: number;
    j = this._terminal.rows - 1 - buffer.scrollBottom;
    j = this._terminal.rows - 1 + buffer.ybase - j;
    while (param--) {
      // test: echo -e '\e[44m\e[1M\e[0m'
      // blankLine(true) - xterm/linux behavior
      buffer.lines.splice(row, 1);
      buffer.lines.splice(j, 0, buffer.getBlankLine(this._terminal.eraseAttrData()));
    }

    // this.maxRange();
    this._terminal.updateRange(buffer.y);
    this._terminal.updateRange(buffer.scrollBottom);
  }

  /**
   * CSI Ps P
   * Delete Ps Character(s) (default = 1) (DCH).
   */
  public deleteChars(params: number[]): void {
    this._terminal.buffer.lines.get(this._terminal.buffer.y + this._terminal.buffer.ybase).deleteCells(
      this._terminal.buffer.x,
      params[0] || 1,
      this._terminal.buffer.getNullCell(this._terminal.eraseAttrData())
    );
    this._terminal.updateRange(this._terminal.buffer.y);
  }

  /**
   * CSI Ps S  Scroll up Ps lines (default = 1) (SU).
   */
  public scrollUp(params: number[]): void {
    let param = params[0] || 1;

    // make buffer local for faster access
    const buffer = this._terminal.buffer;

    while (param--) {
      buffer.lines.splice(buffer.ybase + buffer.scrollTop, 1);
      buffer.lines.splice(buffer.ybase + buffer.scrollBottom, 0, buffer.getBlankLine(DEFAULT_ATTR_DATA));
    }
    // this.maxRange();
    this._terminal.updateRange(buffer.scrollTop);
    this._terminal.updateRange(buffer.scrollBottom);
  }

  /**
   * CSI Ps T  Scroll down Ps lines (default = 1) (SD).
   */
  public scrollDown(params: number[], collect?: string): void {
    if (params.length < 2 && !collect) {
      let param = params[0] || 1;

      // make buffer local for faster access
      const buffer = this._terminal.buffer;

      while (param--) {
        buffer.lines.splice(buffer.ybase + buffer.scrollBottom, 1);
        buffer.lines.splice(buffer.ybase + buffer.scrollTop, 0, buffer.getBlankLine(DEFAULT_ATTR_DATA));
      }
      // this.maxRange();
      this._terminal.updateRange(buffer.scrollTop);
      this._terminal.updateRange(buffer.scrollBottom);
    }
  }

  /**
   * CSI Ps X
   * Erase Ps Character(s) (default = 1) (ECH).
   */
  public eraseChars(params: number[]): void {
    this._terminal.buffer.lines.get(this._terminal.buffer.y + this._terminal.buffer.ybase).replaceCells(
      this._terminal.buffer.x,
      this._terminal.buffer.x + (params[0] || 1),
      this._terminal.buffer.getNullCell(this._terminal.eraseAttrData())
    );
  }

  /**
   * CSI Ps Z  Cursor Backward Tabulation Ps tab stops (default = 1) (CBT).
   */
  public cursorBackwardTab(params: number[]): void {
    let param = params[0] || 1;

    // make buffer local for faster access
    const buffer = this._terminal.buffer;

    while (param--) {
      buffer.x = buffer.prevStop();
    }
  }

  /**
   * CSI Pm `  Character Position Absolute
   *   [column] (default = [row,1]) (HPA).
   */
  public charPosAbsolute(params: number[]): void {
    let param = params[0];
    if (param < 1) {
      param = 1;
    }
    this._terminal.buffer.x = param - 1;
    if (this._terminal.buffer.x >= this._terminal.cols) {
      this._terminal.buffer.x = this._terminal.cols - 1;
    }
  }

  /**
   * CSI Pm a  Character Position Relative
   *   [columns] (default = [row,col+1]) (HPR)
   * reuse CSI Ps C ?
   */
  public hPositionRelative(params: number[]): void {
    let param = params[0];
    if (param < 1) {
      param = 1;
    }
    this._terminal.buffer.x += param;
    if (this._terminal.buffer.x >= this._terminal.cols) {
      this._terminal.buffer.x = this._terminal.cols - 1;
    }
  }

  /**
   * CSI Ps b  Repeat the preceding graphic character Ps times (REP).
   */
  public repeatPrecedingCharacter(params: number[]): void {
    // make buffer local for faster access
    const buffer = this._terminal.buffer;
    const line = buffer.lines.get(buffer.ybase + buffer.y);
    line.loadCell(buffer.x - 1, this._workCell);
    line.replaceCells(buffer.x,
      buffer.x + (params[0] || 1),
      (this._workCell.content !== undefined) ? this._workCell : buffer.getNullCell(DEFAULT_ATTR_DATA)
    );
    // FIXME: no updateRange here?
  }

  /**
   * CSI Ps c  Send Device Attributes (Primary DA).
   *     Ps = 0  or omitted -> request attributes from terminal.  The
   *     response depends on the decTerminalID resource setting.
   *     -> CSI ? 1 ; 2 c  (``VT100 with Advanced Video Option'')
   *     -> CSI ? 1 ; 0 c  (``VT101 with No Options'')
   *     -> CSI ? 6 c  (``VT102'')
   *     -> CSI ? 6 0 ; 1 ; 2 ; 6 ; 8 ; 9 ; 1 5 ; c  (``VT220'')
   *   The VT100-style response parameters do not mean anything by
   *   themselves.  VT220 parameters do, telling the host what fea-
   *   tures the terminal supports:
   *     Ps = 1  -> 132-columns.
   *     Ps = 2  -> Printer.
   *     Ps = 6  -> Selective erase.
   *     Ps = 8  -> User-defined keys.
   *     Ps = 9  -> National replacement character sets.
   *     Ps = 1 5  -> Technical characters.
   *     Ps = 2 2  -> ANSI color, e.g., VT525.
   *     Ps = 2 9  -> ANSI text locator (i.e., DEC Locator mode).
   * CSI > Ps c
   *   Send Device Attributes (Secondary DA).
   *     Ps = 0  or omitted -> request the terminal's identification
   *     code.  The response depends on the decTerminalID resource set-
   *     ting.  It should apply only to VT220 and up, but xterm extends
   *     this to VT100.
   *     -> CSI  > Pp ; Pv ; Pc c
   *   where Pp denotes the terminal type
   *     Pp = 0  -> ``VT100''.
   *     Pp = 1  -> ``VT220''.
   *   and Pv is the firmware version (for xterm, this was originally
   *   the XFree86 patch number, starting with 95).  In a DEC termi-
   *   nal, Pc indicates the ROM cartridge registration number and is
   *   always zero.
   * More information:
   *   xterm/charproc.c - line 2012, for more information.
   *   vim responds with ^[[?0c or ^[[?1c after the terminal's response (?)
   */
  public sendDeviceAttributes(params: number[], collect?: string): void {
    if (params[0] > 0) {
      return;
    }

    if (!collect) {
      if (this._terminal.is('xterm') || this._terminal.is('rxvt-unicode') || this._terminal.is('screen')) {
        this._terminal.handler(C0.ESC + '[?1;2c');
      } else if (this._terminal.is('linux')) {
        this._terminal.handler(C0.ESC + '[?6c');
      }
    } else if (collect === '>') {
      // xterm and urxvt
      // seem to spit this
      // out around ~370 times (?).
      if (this._terminal.is('xterm')) {
        this._terminal.handler(C0.ESC + '[>0;276;0c');
      } else if (this._terminal.is('rxvt-unicode')) {
        this._terminal.handler(C0.ESC + '[>85;95;0c');
      } else if (this._terminal.is('linux')) {
        // not supported by linux console.
        // linux console echoes parameters.
        this._terminal.handler(params[0] + 'c');
      } else if (this._terminal.is('screen')) {
        this._terminal.handler(C0.ESC + '[>83;40003;0c');
      }
    }
  }

  /**
   * CSI Pm d  Vertical Position Absolute (VPA)
   *   [row] (default = [1,column])
   */
  public linePosAbsolute(params: number[]): void {
    let param = params[0];
    if (param < 1) {
      param = 1;
    }
    this._terminal.buffer.y = param - 1;
    if (this._terminal.buffer.y >= this._terminal.rows) {
      this._terminal.buffer.y = this._terminal.rows - 1;
    }
  }

  /**
   * CSI Pm e  Vertical Position Relative (VPR)
   *   [rows] (default = [row+1,column])
   * reuse CSI Ps B ?
   */
  public vPositionRelative(params: number[]): void {
    let param = params[0];
    if (param < 1) {
      param = 1;
    }
    this._terminal.buffer.y += param;
    if (this._terminal.buffer.y >= this._terminal.rows) {
      this._terminal.buffer.y = this._terminal.rows - 1;
    }
    // If the end of the line is hit, prevent this action from wrapping around to the next line.
    if (this._terminal.buffer.x >= this._terminal.cols) {
      this._terminal.buffer.x--;
    }
  }

  /**
   * CSI Ps ; Ps f
   *   Horizontal and Vertical Position [row;column] (default =
   *   [1,1]) (HVP).
   */
  public hVPosition(params: number[]): void {
    if (params[0] < 1) params[0] = 1;
    if (params[1] < 1) params[1] = 1;

    this._terminal.buffer.y = params[0] - 1;
    if (this._terminal.buffer.y >= this._terminal.rows) {
      this._terminal.buffer.y = this._terminal.rows - 1;
    }

    this._terminal.buffer.x = params[1] - 1;
    if (this._terminal.buffer.x >= this._terminal.cols) {
      this._terminal.buffer.x = this._terminal.cols - 1;
    }
  }

  /**
   * CSI Ps g  Tab Clear (TBC).
   *     Ps = 0  -> Clear Current Column (default).
   *     Ps = 3  -> Clear All.
   * Potentially:
   *   Ps = 2  -> Clear Stops on Line.
   *   http://vt100.net/annarbor/aaa-ug/section6.html
   */
  public tabClear(params: number[]): void {
    const param = params[0];
    if (param <= 0) {
      delete this._terminal.buffer.tabs[this._terminal.buffer.x];
    } else if (param === 3) {
      this._terminal.buffer.tabs = {};
    }
  }

  /**
   * CSI Pm h  Set Mode (SM).
   *     Ps = 2  -> Keyboard Action Mode (AM).
   *     Ps = 4  -> Insert Mode (IRM).
   *     Ps = 1 2  -> Send/receive (SRM).
   *     Ps = 2 0  -> Automatic Newline (LNM).
   * CSI ? Pm h
   *   DEC Private Mode Set (DECSET).
   *     Ps = 1  -> Application Cursor Keys (DECCKM).
   *     Ps = 2  -> Designate USASCII for character sets G0-G3
   *     (DECANM), and set VT100 mode.
   *     Ps = 3  -> 132 Column Mode (DECCOLM).
   *     Ps = 4  -> Smooth (Slow) Scroll (DECSCLM).
   *     Ps = 5  -> Reverse Video (DECSCNM).
   *     Ps = 6  -> Origin Mode (DECOM).
   *     Ps = 7  -> Wraparound Mode (DECAWM).
   *     Ps = 8  -> Auto-repeat Keys (DECARM).
   *     Ps = 9  -> Send Mouse X & Y on button press.  See the sec-
   *     tion Mouse Tracking.
   *     Ps = 1 0  -> Show toolbar (rxvt).
   *     Ps = 1 2  -> Start Blinking Cursor (att610).
   *     Ps = 1 8  -> Print form feed (DECPFF).
   *     Ps = 1 9  -> Set print extent to full screen (DECPEX).
   *     Ps = 2 5  -> Show Cursor (DECTCEM).
   *     Ps = 3 0  -> Show scrollbar (rxvt).
   *     Ps = 3 5  -> Enable font-shifting functions (rxvt).
   *     Ps = 3 8  -> Enter Tektronix Mode (DECTEK).
   *     Ps = 4 0  -> Allow 80 -> 132 Mode.
   *     Ps = 4 1  -> more(1) fix (see curses resource).
   *     Ps = 4 2  -> Enable Nation Replacement Character sets (DECN-
   *     RCM).
   *     Ps = 4 4  -> Turn On Margin Bell.
   *     Ps = 4 5  -> Reverse-wraparound Mode.
   *     Ps = 4 6  -> Start Logging.  This is normally disabled by a
   *     compile-time option.
   *     Ps = 4 7  -> Use Alternate Screen Buffer.  (This may be dis-
   *     abled by the titeInhibit resource).
   *     Ps = 6 6  -> Application keypad (DECNKM).
   *     Ps = 6 7  -> Backarrow key sends backspace (DECBKM).
   *     Ps = 1 0 0 0  -> Send Mouse X & Y on button press and
   *     release.  See the section Mouse Tracking.
   *     Ps = 1 0 0 1  -> Use Hilite Mouse Tracking.
   *     Ps = 1 0 0 2  -> Use Cell Motion Mouse Tracking.
   *     Ps = 1 0 0 3  -> Use All Motion Mouse Tracking.
   *     Ps = 1 0 0 4  -> Send FocusIn/FocusOut events.
   *     Ps = 1 0 0 5  -> Enable Extended Mouse Mode.
   *     Ps = 1 0 1 0  -> Scroll to bottom on tty output (rxvt).
   *     Ps = 1 0 1 1  -> Scroll to bottom on key press (rxvt).
   *     Ps = 1 0 3 4  -> Interpret "meta" key, sets eighth bit.
   *     (enables the eightBitInput resource).
   *     Ps = 1 0 3 5  -> Enable special modifiers for Alt and Num-
   *     Lock keys.  (This enables the numLock resource).
   *     Ps = 1 0 3 6  -> Send ESC   when Meta modifies a key.  (This
   *     enables the metaSendsEscape resource).
   *     Ps = 1 0 3 7  -> Send DEL from the editing-keypad Delete
   *     key.
   *     Ps = 1 0 3 9  -> Send ESC  when Alt modifies a key.  (This
   *     enables the altSendsEscape resource).
   *     Ps = 1 0 4 0  -> Keep selection even if not highlighted.
   *     (This enables the keepSelection resource).
   *     Ps = 1 0 4 1  -> Use the CLIPBOARD selection.  (This enables
   *     the selectToClipboard resource).
   *     Ps = 1 0 4 2  -> Enable Urgency window manager hint when
   *     Control-G is received.  (This enables the bellIsUrgent
   *     resource).
   *     Ps = 1 0 4 3  -> Enable raising of the window when Control-G
   *     is received.  (enables the popOnBell resource).
   *     Ps = 1 0 4 7  -> Use Alternate Screen Buffer.  (This may be
   *     disabled by the titeInhibit resource).
   *     Ps = 1 0 4 8  -> Save cursor as in DECSC.  (This may be dis-
   *     abled by the titeInhibit resource).
   *     Ps = 1 0 4 9  -> Save cursor as in DECSC and use Alternate
   *     Screen Buffer, clearing it first.  (This may be disabled by
   *     the titeInhibit resource).  This combines the effects of the 1
   *     0 4 7  and 1 0 4 8  modes.  Use this with terminfo-based
   *     applications rather than the 4 7  mode.
   *     Ps = 1 0 5 0  -> Set terminfo/termcap function-key mode.
   *     Ps = 1 0 5 1  -> Set Sun function-key mode.
   *     Ps = 1 0 5 2  -> Set HP function-key mode.
   *     Ps = 1 0 5 3  -> Set SCO function-key mode.
   *     Ps = 1 0 6 0  -> Set legacy keyboard emulation (X11R6).
   *     Ps = 1 0 6 1  -> Set VT220 keyboard emulation.
   *     Ps = 2 0 0 4  -> Set bracketed paste mode.
   * Modes:
   *   http: *vt100.net/docs/vt220-rm/chapter4.html
   */
  public setMode(params: number[], collect?: string): void {
    if (params.length > 1) {
      for (let i = 0; i < params.length; i++) {
        this.setMode([params[i]]);
      }

      return;
    }

    if (!collect) {
      switch (params[0]) {
        case 4:
          this._terminal.insertMode = true;
          break;
        case 20:
          // this._t.convertEol = true;
          break;
      }
    } else if (collect === '?') {
      switch (params[0]) {
        case 1:
          this._terminal.applicationCursor = true;
          break;
        case 2:
          this._terminal.setgCharset(0, DEFAULT_CHARSET);
          this._terminal.setgCharset(1, DEFAULT_CHARSET);
          this._terminal.setgCharset(2, DEFAULT_CHARSET);
          this._terminal.setgCharset(3, DEFAULT_CHARSET);
          // set VT100 mode here
          break;
        case 3: // 132 col mode
          this._terminal.savedCols = this._terminal.cols;
          this._terminal.resize(132, this._terminal.rows);
          break;
        case 6:
          this._terminal.originMode = true;
          break;
        case 7:
          this._terminal.wraparoundMode = true;
          break;
        case 12:
          // this.cursorBlink = true;
          break;
        case 66:
          this._terminal.log('Serial port requested application keypad.');
          this._terminal.applicationKeypad = true;
          if (this._terminal.viewport) {
            this._terminal.viewport.syncScrollArea();
          }
          break;
        case 9: // X10 Mouse
          // no release, no motion, no wheel, no modifiers.
        case 1000: // vt200 mouse
          // no motion.
          // no modifiers, except control on the wheel.
        case 1002: // button event mouse
        case 1003: // any event mouse
          // any event - sends motion events,
          // even if there is no button held down.

          // TODO: Why are params[0] compares nested within a switch for params[0]?

          this._terminal.x10Mouse = params[0] === 9;
          this._terminal.vt200Mouse = params[0] === 1000;
          this._terminal.normalMouse = params[0] > 1000;
          this._terminal.mouseEvents = true;
          if (this._terminal.element) {
            this._terminal.element.classList.add('enable-mouse-events');
          }
          if (this._terminal.selectionManager) {
            this._terminal.selectionManager.disable();
          }
          this._terminal.log('Binding to mouse events.');
          break;
        case 1004: // send focusin/focusout events
          // focusin: ^[[I
          // focusout: ^[[O
          this._terminal.sendFocus = true;
          break;
        case 1005: // utf8 ext mode mouse
          this._terminal.utfMouse = true;
          // for wide terminals
          // simply encodes large values as utf8 characters
          break;
        case 1006: // sgr ext mode mouse
          this._terminal.sgrMouse = true;
          // for wide terminals
          // does not add 32 to fields
          // press: ^[[<b;x;yM
          // release: ^[[<b;x;ym
          break;
        case 1015: // urxvt ext mode mouse
          this._terminal.urxvtMouse = true;
          // for wide terminals
          // numbers for fields
          // press: ^[[b;x;yM
          // motion: ^[[b;x;yT
          break;
        case 25: // show cursor
          this._terminal.cursorHidden = false;
          break;
        case 1048: // alt screen cursor
          this.saveCursor(params);
          break;
        case 1049: // alt screen buffer cursor
          this.saveCursor(params);
          // FALL-THROUGH
        case 47: // alt screen buffer
        case 1047: // alt screen buffer
          this._terminal.buffers.activateAltBuffer(this._terminal.eraseAttrData());
          this._terminal.refresh(0, this._terminal.rows - 1);
          if (this._terminal.viewport) {
            this._terminal.viewport.syncScrollArea();
          }
          this._terminal.showCursor();
          break;
        case 2004: // bracketed paste mode (https://cirw.in/blog/bracketed-paste)
          this._terminal.bracketedPasteMode = true;
          break;
      }
    }
  }

  /**
   * CSI Pm l  Reset Mode (RM).
   *     Ps = 2  -> Keyboard Action Mode (AM).
   *     Ps = 4  -> Replace Mode (IRM).
   *     Ps = 1 2  -> Send/receive (SRM).
   *     Ps = 2 0  -> Normal Linefeed (LNM).
   * CSI ? Pm l
   *   DEC Private Mode Reset (DECRST).
   *     Ps = 1  -> Normal Cursor Keys (DECCKM).
   *     Ps = 2  -> Designate VT52 mode (DECANM).
   *     Ps = 3  -> 80 Column Mode (DECCOLM).
   *     Ps = 4  -> Jump (Fast) Scroll (DECSCLM).
   *     Ps = 5  -> Normal Video (DECSCNM).
   *     Ps = 6  -> Normal Cursor Mode (DECOM).
   *     Ps = 7  -> No Wraparound Mode (DECAWM).
   *     Ps = 8  -> No Auto-repeat Keys (DECARM).
   *     Ps = 9  -> Don't send Mouse X & Y on button press.
   *     Ps = 1 0  -> Hide toolbar (rxvt).
   *     Ps = 1 2  -> Stop Blinking Cursor (att610).
   *     Ps = 1 8  -> Don't print form feed (DECPFF).
   *     Ps = 1 9  -> Limit print to scrolling region (DECPEX).
   *     Ps = 2 5  -> Hide Cursor (DECTCEM).
   *     Ps = 3 0  -> Don't show scrollbar (rxvt).
   *     Ps = 3 5  -> Disable font-shifting functions (rxvt).
   *     Ps = 4 0  -> Disallow 80 -> 132 Mode.
   *     Ps = 4 1  -> No more(1) fix (see curses resource).
   *     Ps = 4 2  -> Disable Nation Replacement Character sets (DEC-
   *     NRCM).
   *     Ps = 4 4  -> Turn Off Margin Bell.
   *     Ps = 4 5  -> No Reverse-wraparound Mode.
   *     Ps = 4 6  -> Stop Logging.  (This is normally disabled by a
   *     compile-time option).
   *     Ps = 4 7  -> Use Normal Screen Buffer.
   *     Ps = 6 6  -> Numeric keypad (DECNKM).
   *     Ps = 6 7  -> Backarrow key sends delete (DECBKM).
   *     Ps = 1 0 0 0  -> Don't send Mouse X & Y on button press and
   *     release.  See the section Mouse Tracking.
   *     Ps = 1 0 0 1  -> Don't use Hilite Mouse Tracking.
   *     Ps = 1 0 0 2  -> Don't use Cell Motion Mouse Tracking.
   *     Ps = 1 0 0 3  -> Don't use All Motion Mouse Tracking.
   *     Ps = 1 0 0 4  -> Don't send FocusIn/FocusOut events.
   *     Ps = 1 0 0 5  -> Disable Extended Mouse Mode.
   *     Ps = 1 0 1 0  -> Don't scroll to bottom on tty output
   *     (rxvt).
   *     Ps = 1 0 1 1  -> Don't scroll to bottom on key press (rxvt).
   *     Ps = 1 0 3 4  -> Don't interpret "meta" key.  (This disables
   *     the eightBitInput resource).
   *     Ps = 1 0 3 5  -> Disable special modifiers for Alt and Num-
   *     Lock keys.  (This disables the numLock resource).
   *     Ps = 1 0 3 6  -> Don't send ESC  when Meta modifies a key.
   *     (This disables the metaSendsEscape resource).
   *     Ps = 1 0 3 7  -> Send VT220 Remove from the editing-keypad
   *     Delete key.
   *     Ps = 1 0 3 9  -> Don't send ESC  when Alt modifies a key.
   *     (This disables the altSendsEscape resource).
   *     Ps = 1 0 4 0  -> Do not keep selection when not highlighted.
   *     (This disables the keepSelection resource).
   *     Ps = 1 0 4 1  -> Use the PRIMARY selection.  (This disables
   *     the selectToClipboard resource).
   *     Ps = 1 0 4 2  -> Disable Urgency window manager hint when
   *     Control-G is received.  (This disables the bellIsUrgent
   *     resource).
   *     Ps = 1 0 4 3  -> Disable raising of the window when Control-
   *     G is received.  (This disables the popOnBell resource).
   *     Ps = 1 0 4 7  -> Use Normal Screen Buffer, clearing screen
   *     first if in the Alternate Screen.  (This may be disabled by
   *     the titeInhibit resource).
   *     Ps = 1 0 4 8  -> Restore cursor as in DECRC.  (This may be
   *     disabled by the titeInhibit resource).
   *     Ps = 1 0 4 9  -> Use Normal Screen Buffer and restore cursor
   *     as in DECRC.  (This may be disabled by the titeInhibit
   *     resource).  This combines the effects of the 1 0 4 7  and 1 0
   *     4 8  modes.  Use this with terminfo-based applications rather
   *     than the 4 7  mode.
   *     Ps = 1 0 5 0  -> Reset terminfo/termcap function-key mode.
   *     Ps = 1 0 5 1  -> Reset Sun function-key mode.
   *     Ps = 1 0 5 2  -> Reset HP function-key mode.
   *     Ps = 1 0 5 3  -> Reset SCO function-key mode.
   *     Ps = 1 0 6 0  -> Reset legacy keyboard emulation (X11R6).
   *     Ps = 1 0 6 1  -> Reset keyboard emulation to Sun/PC style.
   *     Ps = 2 0 0 4  -> Reset bracketed paste mode.
   */
  public resetMode(params: number[], collect?: string): void {
    if (params.length > 1) {
      for (let i = 0; i < params.length; i++) {
        this.resetMode([params[i]]);
      }

      return;
    }

    if (!collect) {
      switch (params[0]) {
        case 4:
          this._terminal.insertMode = false;
          break;
        case 20:
          // this._t.convertEol = false;
          break;
      }
    } else if (collect === '?') {
      switch (params[0]) {
        case 1:
          this._terminal.applicationCursor = false;
          break;
        case 3:
          if (this._terminal.cols === 132 && this._terminal.savedCols) {
            this._terminal.resize(this._terminal.savedCols, this._terminal.rows);
          }
          delete this._terminal.savedCols;
          break;
        case 6:
          this._terminal.originMode = false;
          break;
        case 7:
          this._terminal.wraparoundMode = false;
          break;
        case 12:
          // this.cursorBlink = false;
          break;
        case 66:
          this._terminal.log('Switching back to normal keypad.');
          this._terminal.applicationKeypad = false;
          if (this._terminal.viewport) {
            this._terminal.viewport.syncScrollArea();
          }
          break;
        case 9: // X10 Mouse
        case 1000: // vt200 mouse
        case 1002: // button event mouse
        case 1003: // any event mouse
          this._terminal.x10Mouse = false;
          this._terminal.vt200Mouse = false;
          this._terminal.normalMouse = false;
          this._terminal.mouseEvents = false;
          if (this._terminal.element) {
            this._terminal.element.classList.remove('enable-mouse-events');
          }
          if (this._terminal.selectionManager) {
            this._terminal.selectionManager.enable();
          }
          break;
        case 1004: // send focusin/focusout events
          this._terminal.sendFocus = false;
          break;
        case 1005: // utf8 ext mode mouse
          this._terminal.utfMouse = false;
          break;
        case 1006: // sgr ext mode mouse
          this._terminal.sgrMouse = false;
          break;
        case 1015: // urxvt ext mode mouse
          this._terminal.urxvtMouse = false;
          break;
        case 25: // hide cursor
          this._terminal.cursorHidden = true;
          break;
        case 1048: // alt screen cursor
          this.restoreCursor(params);
          break;
        case 1049: // alt screen buffer cursor
           // FALL-THROUGH
        case 47: // normal screen buffer
        case 1047: // normal screen buffer - clearing it first
          // Ensure the selection manager has the correct buffer
          this._terminal.buffers.activateNormalBuffer();
          if (params[0] === 1049) {
            this.restoreCursor(params);
          }
          this._terminal.refresh(0, this._terminal.rows - 1);
          if (this._terminal.viewport) {
            this._terminal.viewport.syncScrollArea();
          }
          this._terminal.showCursor();
          break;
        case 2004: // bracketed paste mode (https://cirw.in/blog/bracketed-paste)
          this._terminal.bracketedPasteMode = false;
          break;
      }
    }
  }

  /**
   * CSI Pm m  Character Attributes (SGR).
   *     Ps = 0  -> Normal (default).
   *     Ps = 1  -> Bold.
   *     Ps = 2  -> Faint, decreased intensity (ISO 6429).
   *     Ps = 4  -> Underlined.
   *     Ps = 5  -> Blink (appears as Bold).
   *     Ps = 7  -> Inverse.
   *     Ps = 8  -> Invisible, i.e., hidden (VT300).
   *     Ps = 2 2  -> Normal (neither bold nor faint).
   *     Ps = 2 4  -> Not underlined.
   *     Ps = 2 5  -> Steady (not blinking).
   *     Ps = 2 7  -> Positive (not inverse).
   *     Ps = 2 8  -> Visible, i.e., not hidden (VT300).
   *     Ps = 3 0  -> Set foreground color to Black.
   *     Ps = 3 1  -> Set foreground color to Red.
   *     Ps = 3 2  -> Set foreground color to Green.
   *     Ps = 3 3  -> Set foreground color to Yellow.
   *     Ps = 3 4  -> Set foreground color to Blue.
   *     Ps = 3 5  -> Set foreground color to Magenta.
   *     Ps = 3 6  -> Set foreground color to Cyan.
   *     Ps = 3 7  -> Set foreground color to White.
   *     Ps = 3 9  -> Set foreground color to default (original).
   *     Ps = 4 0  -> Set background color to Black.
   *     Ps = 4 1  -> Set background color to Red.
   *     Ps = 4 2  -> Set background color to Green.
   *     Ps = 4 3  -> Set background color to Yellow.
   *     Ps = 4 4  -> Set background color to Blue.
   *     Ps = 4 5  -> Set background color to Magenta.
   *     Ps = 4 6  -> Set background color to Cyan.
   *     Ps = 4 7  -> Set background color to White.
   *     Ps = 4 9  -> Set background color to default (original).
   *
   *   If 16-color support is compiled, the following apply.  Assume
   *   that xterm's resources are set so that the ISO color codes are
   *   the first 8 of a set of 16.  Then the aixterm colors are the
   *   bright versions of the ISO colors:
   *     Ps = 9 0  -> Set foreground color to Black.
   *     Ps = 9 1  -> Set foreground color to Red.
   *     Ps = 9 2  -> Set foreground color to Green.
   *     Ps = 9 3  -> Set foreground color to Yellow.
   *     Ps = 9 4  -> Set foreground color to Blue.
   *     Ps = 9 5  -> Set foreground color to Magenta.
   *     Ps = 9 6  -> Set foreground color to Cyan.
   *     Ps = 9 7  -> Set foreground color to White.
   *     Ps = 1 0 0  -> Set background color to Black.
   *     Ps = 1 0 1  -> Set background color to Red.
   *     Ps = 1 0 2  -> Set background color to Green.
   *     Ps = 1 0 3  -> Set background color to Yellow.
   *     Ps = 1 0 4  -> Set background color to Blue.
   *     Ps = 1 0 5  -> Set background color to Magenta.
   *     Ps = 1 0 6  -> Set background color to Cyan.
   *     Ps = 1 0 7  -> Set background color to White.
   *
   *   If xterm is compiled with the 16-color support disabled, it
   *   supports the following, from rxvt:
   *     Ps = 1 0 0  -> Set foreground and background color to
   *     default.
   *
   *   If 88- or 256-color support is compiled, the following apply.
   *     Ps = 3 8  ; 5  ; Ps -> Set foreground color to the second
   *     Ps.
   *     Ps = 4 8  ; 5  ; Ps -> Set background color to the second
   *     Ps.
   */
  public charAttributes(params: number[]): void {
    // Optimize a single SGR0.
    if (params.length === 1 && params[0] === 0) {
      this._terminal.curAttrData.fg = DEFAULT_ATTR_DATA.fg;
      this._terminal.curAttrData.bg = DEFAULT_ATTR_DATA.bg;
      return;
    }

    const l = params.length;
    let p;
    const attr = this._terminal.curAttrData;

    for (let i = 0; i < l; i++) {
      p = params[i];
      if (p >= 30 && p <= 37) {
        // fg color 8
        attr.fg &= ~(Attributes.CM_MASK | Attributes.PCOLOR_MASK);
        attr.fg |= Attributes.CM_P16 | (p - 30);
      } else if (p >= 40 && p <= 47) {
        // bg color 8
        attr.bg &= ~(Attributes.CM_MASK | Attributes.PCOLOR_MASK);
        attr.bg |= Attributes.CM_P16 | (p - 40);
      } else if (p >= 90 && p <= 97) {
        // fg color 16
        attr.fg &= ~(Attributes.CM_MASK | Attributes.PCOLOR_MASK);
        attr.fg |= Attributes.CM_P16 | (p - 90) | 8;
      } else if (p >= 100 && p <= 107) {
        // bg color 16
        attr.bg &= ~(Attributes.CM_MASK | Attributes.PCOLOR_MASK);
        attr.bg |= Attributes.CM_P16 | (p - 100) | 8;
      } else if (p === 0) {
        // default
        attr.fg = DEFAULT_ATTR_DATA.fg;
        attr.bg = DEFAULT_ATTR_DATA.bg;
      } else if (p === 1) {
        // bold text
        attr.fg |= FgFlags.BOLD;
      } else if (p === 3) {
        // italic text
        attr.bg |= BgFlags.ITALIC;
      } else if (p === 4) {
        // underlined text
        attr.fg |= FgFlags.UNDERLINE;
      } else if (p === 5) {
        // blink
        attr.fg |= FgFlags.BLINK;
      } else if (p === 7) {
        // inverse and positive
        // test with: echo -e '\e[31m\e[42mhello\e[7mworld\e[27mhi\e[m'
        attr.fg |= FgFlags.INVERSE;
      } else if (p === 8) {
        // invisible
        attr.fg |= FgFlags.INVISIBLE;
      } else if (p === 2) {
        // dimmed text
        attr.bg |= BgFlags.DIM;
      } else if (p === 22) {
        // not bold nor faint
        attr.fg &= ~FgFlags.BOLD;
        attr.bg &= ~BgFlags.DIM;
      } else if (p === 23) {
        // not italic
        attr.bg &= ~BgFlags.ITALIC;
      } else if (p === 24) {
        // not underlined
        attr.fg &= ~FgFlags.UNDERLINE;
      } else if (p === 25) {
        // not blink
        attr.fg &= ~FgFlags.BLINK;
      } else if (p === 27) {
        // not inverse
        attr.fg &= ~FgFlags.INVERSE;
      } else if (p === 28) {
        // not invisible
        attr.fg &= ~FgFlags.INVISIBLE;
      } else if (p === 39) {
        // reset fg
        attr.fg &= ~(Attributes.CM_MASK | Attributes.RGB_MASK);
        attr.fg |= DEFAULT_ATTR_DATA.fg & (Attributes.PCOLOR_MASK | Attributes.RGB_MASK);
      } else if (p === 49) {
        // reset bg
        attr.bg &= ~(Attributes.CM_MASK | Attributes.RGB_MASK);
        attr.bg |= DEFAULT_ATTR_DATA.bg & (Attributes.PCOLOR_MASK | Attributes.RGB_MASK);
      } else if (p === 38) {
        // fg color 256 and RGB
        if (params[i + 1] === 2) {
          i += 2;
          attr.fg |= Attributes.CM_RGB;
          attr.fg &= ~Attributes.RGB_MASK;
          attr.fg |= AttributeData.fromColorRGB([params[i], params[i + 1], params[i + 2]]);
          i += 2;
        } else if (params[i + 1] === 5) {
          i += 2;
          p = params[i] & 0xff;
          attr.fg &= ~(Attributes.CM_MASK | Attributes.PCOLOR_MASK);
          attr.fg |= Attributes.CM_P256 | p;
        }
      } else if (p === 48) {
        // bg color 256 and RGB
        if (params[i + 1] === 2) {
          i += 2;
          attr.bg |= Attributes.CM_RGB;
          attr.bg &= ~Attributes.RGB_MASK;
          attr.bg |= AttributeData.fromColorRGB([params[i], params[i + 1], params[i + 2]]);
          i += 2;
        } else if (params[i + 1] === 5) {
          i += 2;
          p = params[i] & 0xff;
          attr.bg &= ~(Attributes.CM_MASK | Attributes.PCOLOR_MASK);
          attr.bg |= Attributes.CM_P256 | p;
        }
      } else if (p === 100) {
        // reset fg/bg
        attr.fg &= ~(Attributes.CM_MASK | Attributes.RGB_MASK);
        attr.fg |= DEFAULT_ATTR_DATA.fg & (Attributes.PCOLOR_MASK | Attributes.RGB_MASK);
        attr.bg &= ~(Attributes.CM_MASK | Attributes.RGB_MASK);
        attr.bg |= DEFAULT_ATTR_DATA.bg & (Attributes.PCOLOR_MASK | Attributes.RGB_MASK);
      } else {
        this._terminal.error('Unknown SGR attribute: %d.', p);
      }
    }
  }

  /**
   * CSI Ps n  Device Status Report (DSR).
   *     Ps = 5  -> Status Report.  Result (``OK'') is
   *   CSI 0 n
   *     Ps = 6  -> Report Cursor Position (CPR) [row;column].
   *   Result is
   *   CSI r ; c R
   * CSI ? Ps n
   *   Device Status Report (DSR, DEC-specific).
   *     Ps = 6  -> Report Cursor Position (CPR) [row;column] as CSI
   *     ? r ; c R (assumes page is zero).
   *     Ps = 1 5  -> Report Printer status as CSI ? 1 0  n  (ready).
   *     or CSI ? 1 1  n  (not ready).
   *     Ps = 2 5  -> Report UDK status as CSI ? 2 0  n  (unlocked)
   *     or CSI ? 2 1  n  (locked).
   *     Ps = 2 6  -> Report Keyboard status as
   *   CSI ? 2 7  ;  1  ;  0  ;  0  n  (North American).
   *   The last two parameters apply to VT400 & up, and denote key-
   *   board ready and LK01 respectively.
   *     Ps = 5 3  -> Report Locator status as
   *   CSI ? 5 3  n  Locator available, if compiled-in, or
   *   CSI ? 5 0  n  No Locator, if not.
   */
  public deviceStatus(params: number[], collect?: string): void {
    if (!collect) {
      switch (params[0]) {
        case 5:
          // status report
          this._onData.fire(`${C0.ESC}[0n`);
          break;
        case 6:
          // cursor position
          const y = this._terminal.buffer.y + 1;
          const x = this._terminal.buffer.x + 1;
          this._onData.fire(`${C0.ESC}[${y};${x}R`);
          break;
      }
    } else if (collect === '?') {
      // modern xterm doesnt seem to
      // respond to any of these except ?6, 6, and 5
      switch (params[0]) {
        case 6:
          // cursor position
          const y = this._terminal.buffer.y + 1;
          const x = this._terminal.buffer.x + 1;
          this._onData.fire(`${C0.ESC}[?${y};${x}R`);
          break;
        case 15:
          // no printer
          // this.handler(C0.ESC + '[?11n');
          break;
        case 25:
          // dont support user defined keys
          // this.handler(C0.ESC + '[?21n');
          break;
        case 26:
          // north american keyboard
          // this.handler(C0.ESC + '[?27;1;0;0n');
          break;
        case 53:
          // no dec locator/mouse
          // this.handler(C0.ESC + '[?50n');
          break;
      }
    }
  }

  /**
   * CSI ! p   Soft terminal reset (DECSTR).
   * http://vt100.net/docs/vt220-rm/table4-10.html
   */
  public softReset(params: number[], collect?: string): void {
    if (collect === '!') {
      this._terminal.cursorHidden = false;
      this._terminal.insertMode = false;
      this._terminal.originMode = false;
      this._terminal.wraparoundMode = true;  // defaults: xterm - true, vt100 - false
      this._terminal.applicationKeypad = false; // ?
      if (this._terminal.viewport) {
        this._terminal.viewport.syncScrollArea();
      }
      this._terminal.applicationCursor = false;
      this._terminal.buffer.scrollTop = 0;
      this._terminal.buffer.scrollBottom = this._terminal.rows - 1;
      this._terminal.curAttrData = DEFAULT_ATTR_DATA;
      this._terminal.buffer.x = this._terminal.buffer.y = 0; // ?
      this._terminal.charset = null;
      this._terminal.glevel = 0; // ??
      this._terminal.charsets = [null]; // ??
    }
  }

  /**
   * CSI Ps SP q  Set cursor style (DECSCUSR, VT520).
   *   Ps = 0  -> blinking block.
   *   Ps = 1  -> blinking block (default).
   *   Ps = 2  -> steady block.
   *   Ps = 3  -> blinking underline.
   *   Ps = 4  -> steady underline.
   *   Ps = 5  -> blinking bar (xterm).
   *   Ps = 6  -> steady bar (xterm).
   */
  public setCursorStyle(params?: number[], collect?: string): void {
    if (collect === ' ') {
      const param = params[0] < 1 ? 1 : params[0];
      switch (param) {
        case 1:
        case 2:
          this._terminal.setOption('cursorStyle', 'block');
          break;
        case 3:
        case 4:
          this._terminal.setOption('cursorStyle', 'underline');
          break;
        case 5:
        case 6:
          this._terminal.setOption('cursorStyle', 'bar');
          break;
      }
      const isBlinking = param % 2 === 1;
      this._terminal.setOption('cursorBlink', isBlinking);
    }
  }

  /**
   * CSI Ps ; Ps r
   *   Set Scrolling Region [top;bottom] (default = full size of win-
   *   dow) (DECSTBM).
   * CSI ? Pm r
   */
  public setScrollRegion(params: number[], collect?: string): void {
    if (collect) {
      return;
    }
    this._terminal.buffer.scrollTop = (params[0] || 1) - 1;
    this._terminal.buffer.scrollBottom = (params[1] && params[1] <= this._terminal.rows ? params[1] : this._terminal.rows) - 1;
    this._terminal.buffer.x = 0;
    this._terminal.buffer.y = 0;
  }


  /**
   * CSI s
   * ESC 7
   *   Save cursor (ANSI.SYS).
   */
  public saveCursor(params: number[]): void {
    this._terminal.buffer.savedX = this._terminal.buffer.x;
    this._terminal.buffer.savedY = this._terminal.buffer.y;
    this._terminal.buffer.savedCurAttrData.fg = this._terminal.curAttrData.fg;
    this._terminal.buffer.savedCurAttrData.bg = this._terminal.curAttrData.bg;
  }


  /**
   * CSI u
   * ESC 8
   *   Restore cursor (ANSI.SYS).
   */
  public restoreCursor(params: number[]): void {
    this._terminal.buffer.x = this._terminal.buffer.savedX || 0;
    this._terminal.buffer.y = this._terminal.buffer.savedY || 0;
    this._terminal.curAttrData.fg = this._terminal.buffer.savedCurAttrData.fg;
    this._terminal.curAttrData.bg = this._terminal.buffer.savedCurAttrData.bg;
  }


  /**
   * OSC 0; <data> ST (set icon name + window title)
   * OSC 2; <data> ST (set window title)
   *   Proxy to set window title. Icon name is not supported.
   */
  public setTitle(data: string): void {
    this._terminal.handleTitle(data);
  }

  /**
   * ESC E
   * C1.NEL
   *   DEC mnemonic: NEL (https://vt100.net/docs/vt510-rm/NEL)
   *   Moves cursor to first position on next line.
   */
  public nextLine(): void {
    this._terminal.buffer.x = 0;
    this.index();
  }

  /**
   * ESC =
   *   DEC mnemonic: DECKPAM (https://vt100.net/docs/vt510-rm/DECKPAM.html)
   *   Enables the numeric keypad to send application sequences to the host.
   */
  public keypadApplicationMode(): void {
    this._terminal.log('Serial port requested application keypad.');
    this._terminal.applicationKeypad = true;
    if (this._terminal.viewport) {
      this._terminal.viewport.syncScrollArea();
    }
  }

  /**
   * ESC >
   *   DEC mnemonic: DECKPNM (https://vt100.net/docs/vt510-rm/DECKPNM.html)
   *   Enables the keypad to send numeric characters to the host.
   */
  public keypadNumericMode(): void {
    this._terminal.log('Switching back to normal keypad.');
    this._terminal.applicationKeypad = false;
    if (this._terminal.viewport) {
      this._terminal.viewport.syncScrollArea();
    }
  }

  /**
   * ESC % @
   * ESC % G
   *   Select default character set. UTF-8 is not supported (string are unicode anyways)
   *   therefore ESC % G does the same.
   */
  public selectDefaultCharset(): void {
    this._terminal.setgLevel(0);
    this._terminal.setgCharset(0, DEFAULT_CHARSET); // US (default)
  }

  /**
   * ESC ( C
   *   Designate G0 Character Set, VT100, ISO 2022.
   * ESC ) C
   *   Designate G1 Character Set (ISO 2022, VT100).
   * ESC * C
   *   Designate G2 Character Set (ISO 2022, VT220).
   * ESC + C
   *   Designate G3 Character Set (ISO 2022, VT220).
   * ESC - C
   *   Designate G1 Character Set (VT300).
   * ESC . C
   *   Designate G2 Character Set (VT300).
   * ESC / C
   *   Designate G3 Character Set (VT300). C = A  -> ISO Latin-1 Supplemental. - Supported?
   */
  public selectCharset(collectAndFlag: string): void {
    if (collectAndFlag.length !== 2) {
      this.selectDefaultCharset();
      return;
    }
    if (collectAndFlag[0] === '/') {
      return;  // TODO: Is this supported?
    }
    this._terminal.setgCharset(GLEVEL[collectAndFlag[0]], CHARSETS[collectAndFlag[1]] || DEFAULT_CHARSET);
    return;
  }

  /**
   * ESC D
   * C1.IND
   *   DEC mnemonic: IND (https://vt100.net/docs/vt510-rm/IND.html)
   *   Moves the cursor down one line in the same column.
   */
  public index(): void {
    this._terminal.index();  // TODO: save to move from terminal?
  }

  /**
   * ESC H
   * C1.HTS
   *   DEC mnemonic: HTS (https://vt100.net/docs/vt510-rm/HTS.html)
   *   Sets a horizontal tab stop at the column position indicated by
   *   the value of the active column when the terminal receives an HTS.
   */
  public tabSet(): void {
    this._terminal.tabSet();  // TODO: save to move from terminal?
  }

  /**
   * ESC M
   * C1.RI
   *   DEC mnemonic: HTS
   *   Moves the cursor up one line in the same column. If the cursor is at the top margin,
   *   the page scrolls down.
   */
  public reverseIndex(): void {
    this._terminal.reverseIndex();  // TODO: save to move from terminal?
  }

  /**
   * ESC c
   *   DEC mnemonic: RIS (https://vt100.net/docs/vt510-rm/RIS.html)
   *   Reset to initial state.
   */
  public reset(): void {
    this._parser.reset();
    this._terminal.reset();  // TODO: save to move from terminal?
  }

  /**
   * ESC n
   * ESC o
   * ESC |
   * ESC }
   * ESC ~
   *   DEC mnemonic: LS (https://vt100.net/docs/vt510-rm/LS.html)
   *   When you use a locking shift, the character set remains in GL or GR until
   *   you use another locking shift. (partly supported)
   */
  public setgLevel(level: number): void {
    this._terminal.setgLevel(level);  // TODO: save to move from terminal?
  }
}
