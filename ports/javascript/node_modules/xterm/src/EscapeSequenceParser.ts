/**
 * Copyright (c) 2018 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { ParserState, ParserAction, IParsingState, IDcsHandler, IEscapeSequenceParser } from './Types';
import { IDisposable } from 'xterm';
import { Disposable } from './common/Lifecycle';
import { utf32ToString } from './core/input/TextDecoder';

interface IHandlerCollection<T> {
  [key: string]: T[];
}

type CsiHandler = (params: number[], collect: string) => boolean | void;
type OscHandler = (data: string) => boolean | void;

/**
 * Returns an array filled with numbers between the low and high parameters (right exclusive).
 * @param low The low number.
 * @param high The high number.
 */
function r(low: number, high: number): number[] {
  let c = high - low;
  const arr = new Array(c);
  while (c--) {
    arr[c] = --high;
  }
  return arr;
}

/**
 * Transition table for EscapeSequenceParser.
 * NOTE: data in the underlying table is packed like this:
 *   currentState << 8 | characterCode  -->  action << 4 | nextState
 */
export class TransitionTable {
  public table: Uint8Array | number[];

  constructor(length: number) {
    this.table = (typeof Uint8Array === 'undefined')
      ? new Array(length)
      : new Uint8Array(length);
  }

  /**
   * Add a transition to the transition table.
   * @param code input character code
   * @param state current parser state
   * @param action parser action to be done
   * @param next next parser state
   */
  add(code: number, state: number, action: number | null, next: number | null): void {
    this.table[state << 8 | code] = ((action | 0) << 4) | ((next === undefined) ? state : next);
  }

  /**
   * Add transitions for multiple input character codes.
   * @param codes input character code array
   * @param state current parser state
   * @param action parser action to be done
   * @param next next parser state
   */
  addMany(codes: number[], state: number, action: number | null, next: number | null): void {
    for (let i = 0; i < codes.length; i++) {
      this.add(codes[i], state, action, next);
    }
  }
}


/**
 * Default definitions for the VT500_TRANSITION_TABLE.
 */
const PRINTABLES = r(0x20, 0x7f);
const EXECUTABLES = r(0x00, 0x18);
EXECUTABLES.push(0x19);
EXECUTABLES.push.apply(EXECUTABLES, r(0x1c, 0x20));
// Pseudo-character placeholder for printable non-ascii characters.
const NON_ASCII_PRINTABLE = 0xA0;

/**
 * VT500 compatible transition table.
 * Taken from https://vt100.net/emu/dec_ansi_parser.
 */
export const VT500_TRANSITION_TABLE = (function (): TransitionTable {
  const table: TransitionTable = new TransitionTable(4095);

  const states: number[] = r(ParserState.GROUND, ParserState.DCS_PASSTHROUGH + 1);
  let state: any;

  // table with default transition
  for (state in states) {
    // NOTE: table lookup is capped at 0xa0 in parse to keep the table small
    for (let code = 0; code <= NON_ASCII_PRINTABLE; ++code) {
      table.add(code, state, ParserAction.ERROR, ParserState.GROUND);
    }
  }
  // printables
  table.addMany(PRINTABLES, ParserState.GROUND, ParserAction.PRINT, ParserState.GROUND);
  // global anywhere rules
  for (state in states) {
    table.addMany([0x18, 0x1a, 0x99, 0x9a], state, ParserAction.EXECUTE, ParserState.GROUND);
    table.addMany(r(0x80, 0x90), state, ParserAction.EXECUTE, ParserState.GROUND);
    table.addMany(r(0x90, 0x98), state, ParserAction.EXECUTE, ParserState.GROUND);
    table.add(0x9c, state, ParserAction.IGNORE, ParserState.GROUND); // ST as terminator
    table.add(0x1b, state, ParserAction.CLEAR, ParserState.ESCAPE);  // ESC
    table.add(0x9d, state, ParserAction.OSC_START, ParserState.OSC_STRING);  // OSC
    table.addMany([0x98, 0x9e, 0x9f], state, ParserAction.IGNORE, ParserState.SOS_PM_APC_STRING);
    table.add(0x9b, state, ParserAction.CLEAR, ParserState.CSI_ENTRY);  // CSI
    table.add(0x90, state, ParserAction.CLEAR, ParserState.DCS_ENTRY);  // DCS
  }
  // rules for executables and 7f
  table.addMany(EXECUTABLES, ParserState.GROUND, ParserAction.EXECUTE, ParserState.GROUND);
  table.addMany(EXECUTABLES, ParserState.ESCAPE, ParserAction.EXECUTE, ParserState.ESCAPE);
  table.add(0x7f, ParserState.ESCAPE, ParserAction.IGNORE, ParserState.ESCAPE);
  table.addMany(EXECUTABLES, ParserState.OSC_STRING, ParserAction.IGNORE, ParserState.OSC_STRING);
  table.addMany(EXECUTABLES, ParserState.CSI_ENTRY, ParserAction.EXECUTE, ParserState.CSI_ENTRY);
  table.add(0x7f, ParserState.CSI_ENTRY, ParserAction.IGNORE, ParserState.CSI_ENTRY);
  table.addMany(EXECUTABLES, ParserState.CSI_PARAM, ParserAction.EXECUTE, ParserState.CSI_PARAM);
  table.add(0x7f, ParserState.CSI_PARAM, ParserAction.IGNORE, ParserState.CSI_PARAM);
  table.addMany(EXECUTABLES, ParserState.CSI_IGNORE, ParserAction.EXECUTE, ParserState.CSI_IGNORE);
  table.addMany(EXECUTABLES, ParserState.CSI_INTERMEDIATE, ParserAction.EXECUTE, ParserState.CSI_INTERMEDIATE);
  table.add(0x7f, ParserState.CSI_INTERMEDIATE, ParserAction.IGNORE, ParserState.CSI_INTERMEDIATE);
  table.addMany(EXECUTABLES, ParserState.ESCAPE_INTERMEDIATE, ParserAction.EXECUTE, ParserState.ESCAPE_INTERMEDIATE);
  table.add(0x7f, ParserState.ESCAPE_INTERMEDIATE, ParserAction.IGNORE, ParserState.ESCAPE_INTERMEDIATE);
  // osc
  table.add(0x5d, ParserState.ESCAPE, ParserAction.OSC_START, ParserState.OSC_STRING);
  table.addMany(PRINTABLES, ParserState.OSC_STRING, ParserAction.OSC_PUT, ParserState.OSC_STRING);
  table.add(0x7f, ParserState.OSC_STRING, ParserAction.OSC_PUT, ParserState.OSC_STRING);
  table.addMany([0x9c, 0x1b, 0x18, 0x1a, 0x07], ParserState.OSC_STRING, ParserAction.OSC_END, ParserState.GROUND);
  table.addMany(r(0x1c, 0x20), ParserState.OSC_STRING, ParserAction.IGNORE, ParserState.OSC_STRING);
  // sos/pm/apc does nothing
  table.addMany([0x58, 0x5e, 0x5f], ParserState.ESCAPE, ParserAction.IGNORE, ParserState.SOS_PM_APC_STRING);
  table.addMany(PRINTABLES, ParserState.SOS_PM_APC_STRING, ParserAction.IGNORE, ParserState.SOS_PM_APC_STRING);
  table.addMany(EXECUTABLES, ParserState.SOS_PM_APC_STRING, ParserAction.IGNORE, ParserState.SOS_PM_APC_STRING);
  table.add(0x9c, ParserState.SOS_PM_APC_STRING, ParserAction.IGNORE, ParserState.GROUND);
  table.add(0x7f, ParserState.SOS_PM_APC_STRING, ParserAction.IGNORE, ParserState.SOS_PM_APC_STRING);
  // csi entries
  table.add(0x5b, ParserState.ESCAPE, ParserAction.CLEAR, ParserState.CSI_ENTRY);
  table.addMany(r(0x40, 0x7f), ParserState.CSI_ENTRY, ParserAction.CSI_DISPATCH, ParserState.GROUND);
  table.addMany(r(0x30, 0x3a), ParserState.CSI_ENTRY, ParserAction.PARAM, ParserState.CSI_PARAM);
  table.add(0x3b, ParserState.CSI_ENTRY, ParserAction.PARAM, ParserState.CSI_PARAM);
  table.addMany([0x3c, 0x3d, 0x3e, 0x3f], ParserState.CSI_ENTRY, ParserAction.COLLECT, ParserState.CSI_PARAM);
  table.addMany(r(0x30, 0x3a), ParserState.CSI_PARAM, ParserAction.PARAM, ParserState.CSI_PARAM);
  table.add(0x3b, ParserState.CSI_PARAM, ParserAction.PARAM, ParserState.CSI_PARAM);
  table.addMany(r(0x40, 0x7f), ParserState.CSI_PARAM, ParserAction.CSI_DISPATCH, ParserState.GROUND);
  table.addMany([0x3a, 0x3c, 0x3d, 0x3e, 0x3f], ParserState.CSI_PARAM, ParserAction.IGNORE, ParserState.CSI_IGNORE);
  table.addMany(r(0x20, 0x40), ParserState.CSI_IGNORE, ParserAction.IGNORE, ParserState.CSI_IGNORE);
  table.add(0x7f, ParserState.CSI_IGNORE, ParserAction.IGNORE, ParserState.CSI_IGNORE);
  table.addMany(r(0x40, 0x7f), ParserState.CSI_IGNORE, ParserAction.IGNORE, ParserState.GROUND);
  table.add(0x3a, ParserState.CSI_ENTRY, ParserAction.IGNORE, ParserState.CSI_IGNORE);
  table.addMany(r(0x20, 0x30), ParserState.CSI_ENTRY, ParserAction.COLLECT, ParserState.CSI_INTERMEDIATE);
  table.addMany(r(0x20, 0x30), ParserState.CSI_INTERMEDIATE, ParserAction.COLLECT, ParserState.CSI_INTERMEDIATE);
  table.addMany(r(0x30, 0x40), ParserState.CSI_INTERMEDIATE, ParserAction.IGNORE, ParserState.CSI_IGNORE);
  table.addMany(r(0x40, 0x7f), ParserState.CSI_INTERMEDIATE, ParserAction.CSI_DISPATCH, ParserState.GROUND);
  table.addMany(r(0x20, 0x30), ParserState.CSI_PARAM, ParserAction.COLLECT, ParserState.CSI_INTERMEDIATE);
  // esc_intermediate
  table.addMany(r(0x20, 0x30), ParserState.ESCAPE, ParserAction.COLLECT, ParserState.ESCAPE_INTERMEDIATE);
  table.addMany(r(0x20, 0x30), ParserState.ESCAPE_INTERMEDIATE, ParserAction.COLLECT, ParserState.ESCAPE_INTERMEDIATE);
  table.addMany(r(0x30, 0x7f), ParserState.ESCAPE_INTERMEDIATE, ParserAction.ESC_DISPATCH, ParserState.GROUND);
  table.addMany(r(0x30, 0x50), ParserState.ESCAPE, ParserAction.ESC_DISPATCH, ParserState.GROUND);
  table.addMany(r(0x51, 0x58), ParserState.ESCAPE, ParserAction.ESC_DISPATCH, ParserState.GROUND);
  table.addMany([0x59, 0x5a, 0x5c], ParserState.ESCAPE, ParserAction.ESC_DISPATCH, ParserState.GROUND);
  table.addMany(r(0x60, 0x7f), ParserState.ESCAPE, ParserAction.ESC_DISPATCH, ParserState.GROUND);
  // dcs entry
  table.add(0x50, ParserState.ESCAPE, ParserAction.CLEAR, ParserState.DCS_ENTRY);
  table.addMany(EXECUTABLES, ParserState.DCS_ENTRY, ParserAction.IGNORE, ParserState.DCS_ENTRY);
  table.add(0x7f, ParserState.DCS_ENTRY, ParserAction.IGNORE, ParserState.DCS_ENTRY);
  table.addMany(r(0x1c, 0x20), ParserState.DCS_ENTRY, ParserAction.IGNORE, ParserState.DCS_ENTRY);
  table.addMany(r(0x20, 0x30), ParserState.DCS_ENTRY, ParserAction.COLLECT, ParserState.DCS_INTERMEDIATE);
  table.add(0x3a, ParserState.DCS_ENTRY, ParserAction.IGNORE, ParserState.DCS_IGNORE);
  table.addMany(r(0x30, 0x3a), ParserState.DCS_ENTRY, ParserAction.PARAM, ParserState.DCS_PARAM);
  table.add(0x3b, ParserState.DCS_ENTRY, ParserAction.PARAM, ParserState.DCS_PARAM);
  table.addMany([0x3c, 0x3d, 0x3e, 0x3f], ParserState.DCS_ENTRY, ParserAction.COLLECT, ParserState.DCS_PARAM);
  table.addMany(EXECUTABLES, ParserState.DCS_IGNORE, ParserAction.IGNORE, ParserState.DCS_IGNORE);
  table.addMany(r(0x20, 0x80), ParserState.DCS_IGNORE, ParserAction.IGNORE, ParserState.DCS_IGNORE);
  table.addMany(r(0x1c, 0x20), ParserState.DCS_IGNORE, ParserAction.IGNORE, ParserState.DCS_IGNORE);
  table.addMany(EXECUTABLES, ParserState.DCS_PARAM, ParserAction.IGNORE, ParserState.DCS_PARAM);
  table.add(0x7f, ParserState.DCS_PARAM, ParserAction.IGNORE, ParserState.DCS_PARAM);
  table.addMany(r(0x1c, 0x20), ParserState.DCS_PARAM, ParserAction.IGNORE, ParserState.DCS_PARAM);
  table.addMany(r(0x30, 0x3a), ParserState.DCS_PARAM, ParserAction.PARAM, ParserState.DCS_PARAM);
  table.add(0x3b, ParserState.DCS_PARAM, ParserAction.PARAM, ParserState.DCS_PARAM);
  table.addMany([0x3a, 0x3c, 0x3d, 0x3e, 0x3f], ParserState.DCS_PARAM, ParserAction.IGNORE, ParserState.DCS_IGNORE);
  table.addMany(r(0x20, 0x30), ParserState.DCS_PARAM, ParserAction.COLLECT, ParserState.DCS_INTERMEDIATE);
  table.addMany(EXECUTABLES, ParserState.DCS_INTERMEDIATE, ParserAction.IGNORE, ParserState.DCS_INTERMEDIATE);
  table.add(0x7f, ParserState.DCS_INTERMEDIATE, ParserAction.IGNORE, ParserState.DCS_INTERMEDIATE);
  table.addMany(r(0x1c, 0x20), ParserState.DCS_INTERMEDIATE, ParserAction.IGNORE, ParserState.DCS_INTERMEDIATE);
  table.addMany(r(0x20, 0x30), ParserState.DCS_INTERMEDIATE, ParserAction.COLLECT, ParserState.DCS_INTERMEDIATE);
  table.addMany(r(0x30, 0x40), ParserState.DCS_INTERMEDIATE, ParserAction.IGNORE, ParserState.DCS_IGNORE);
  table.addMany(r(0x40, 0x7f), ParserState.DCS_INTERMEDIATE, ParserAction.DCS_HOOK, ParserState.DCS_PASSTHROUGH);
  table.addMany(r(0x40, 0x7f), ParserState.DCS_PARAM, ParserAction.DCS_HOOK, ParserState.DCS_PASSTHROUGH);
  table.addMany(r(0x40, 0x7f), ParserState.DCS_ENTRY, ParserAction.DCS_HOOK, ParserState.DCS_PASSTHROUGH);
  table.addMany(EXECUTABLES, ParserState.DCS_PASSTHROUGH, ParserAction.DCS_PUT, ParserState.DCS_PASSTHROUGH);
  table.addMany(PRINTABLES, ParserState.DCS_PASSTHROUGH, ParserAction.DCS_PUT, ParserState.DCS_PASSTHROUGH);
  table.add(0x7f, ParserState.DCS_PASSTHROUGH, ParserAction.IGNORE, ParserState.DCS_PASSTHROUGH);
  table.addMany([0x1b, 0x9c], ParserState.DCS_PASSTHROUGH, ParserAction.DCS_UNHOOK, ParserState.GROUND);
  table.add(NON_ASCII_PRINTABLE, ParserState.OSC_STRING, ParserAction.OSC_PUT, ParserState.OSC_STRING);
  return table;
})();

/**
 * Dummy DCS handler as default fallback.
 */
class DcsDummy implements IDcsHandler {
  hook(collect: string, params: number[], flag: number): void { }
  put(data: Uint32Array, start: number, end: number): void { }
  unhook(): void { }
}

/**
 * EscapeSequenceParser.
 * This class implements the ANSI/DEC compatible parser described by
 * Paul Williams (https://vt100.net/emu/dec_ansi_parser).
 * To implement custom ANSI compliant escape sequences it is not needed to
 * alter this parser, instead consider registering a custom handler.
 * For non ANSI compliant sequences change the transition table with
 * the optional `transitions` contructor argument and
 * reimplement the `parse` method.
 * NOTE: The parameter element notation is currently not supported.
 * TODO: implement error recovery hook via error handler return values
 */
export class EscapeSequenceParser extends Disposable implements IEscapeSequenceParser {
  public initialState: number;
  public currentState: number;

  // buffers over several parse calls
  protected _osc: string;
  protected _params: number[];
  protected _collect: string;

  // handler lookup containers
  protected _printHandler: (data: Uint32Array, start: number, end: number) => void;
  protected _executeHandlers: any;
  protected _csiHandlers: IHandlerCollection<CsiHandler>;
  protected _escHandlers: any;
  protected _oscHandlers: IHandlerCollection<OscHandler>;
  protected _dcsHandlers: any;
  protected _activeDcsHandler: IDcsHandler | null;
  protected _errorHandler: (state: IParsingState) => IParsingState;

  // fallback handlers
  protected _printHandlerFb: (data: Uint32Array, start: number, end: number) => void;
  protected _executeHandlerFb: (code: number) => void;
  protected _csiHandlerFb: (collect: string, params: number[], flag: number) => void;
  protected _escHandlerFb: (collect: string, flag: number) => void;
  protected _oscHandlerFb: (identifier: number, data: string) => void;
  protected _dcsHandlerFb: IDcsHandler;
  protected _errorHandlerFb: (state: IParsingState) => IParsingState;

  constructor(readonly TRANSITIONS: TransitionTable = VT500_TRANSITION_TABLE) {
    super();

    this.initialState = ParserState.GROUND;
    this.currentState = this.initialState;
    this._osc = '';
    this._params = [0];
    this._collect = '';

    // set default fallback handlers and handler lookup containers
    this._printHandlerFb = (data, start, end): void => { };
    this._executeHandlerFb = (code: number): void => { };
    this._csiHandlerFb = (collect: string, params: number[], flag: number): void => { };
    this._escHandlerFb = (collect: string, flag: number): void => { };
    this._oscHandlerFb = (identifier: number, data: string): void => { };
    this._dcsHandlerFb = new DcsDummy();
    this._errorHandlerFb = (state: IParsingState): IParsingState => state;
    this._printHandler = this._printHandlerFb;
    this._executeHandlers = Object.create(null);
    this._csiHandlers = Object.create(null);
    this._escHandlers = Object.create(null);
    this._oscHandlers = Object.create(null);
    this._dcsHandlers = Object.create(null);
    this._activeDcsHandler = null;
    this._errorHandler = this._errorHandlerFb;

    // swallow 7bit ST (ESC+\)
    this.setEscHandler('\\', () => {});
  }

  public dispose(): void {
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
  }

  setPrintHandler(callback: (data: Uint32Array, start: number, end: number) => void): void {
    this._printHandler = callback;
  }
  clearPrintHandler(): void {
    this._printHandler = this._printHandlerFb;
  }

  setExecuteHandler(flag: string, callback: () => void): void {
    this._executeHandlers[flag.charCodeAt(0)] = callback;
  }
  clearExecuteHandler(flag: string): void {
    if (this._executeHandlers[flag.charCodeAt(0)]) delete this._executeHandlers[flag.charCodeAt(0)];
  }
  setExecuteHandlerFallback(callback: (code: number) => void): void {
    this._executeHandlerFb = callback;
  }

  addCsiHandler(flag: string, callback: CsiHandler): IDisposable {
    const index = flag.charCodeAt(0);
    if (this._csiHandlers[index] === undefined) {
      this._csiHandlers[index] = [];
    }
    const handlerList = this._csiHandlers[index];
    handlerList.push(callback);
    return {
      dispose: () => {
        const handlerIndex = handlerList.indexOf(callback);
        if (handlerIndex !== -1) {
          handlerList.splice(handlerIndex, 1);
        }
      }
    };
  }
  setCsiHandler(flag: string, callback: (params: number[], collect: string) => void): void {
    this._csiHandlers[flag.charCodeAt(0)] = [callback];
  }
  clearCsiHandler(flag: string): void {
    if (this._csiHandlers[flag.charCodeAt(0)]) delete this._csiHandlers[flag.charCodeAt(0)];
  }
  setCsiHandlerFallback(callback: (collect: string, params: number[], flag: number) => void): void {
    this._csiHandlerFb = callback;
  }

  setEscHandler(collectAndFlag: string, callback: () => void): void {
    this._escHandlers[collectAndFlag] = callback;
  }
  clearEscHandler(collectAndFlag: string): void {
    if (this._escHandlers[collectAndFlag]) delete this._escHandlers[collectAndFlag];
  }
  setEscHandlerFallback(callback: (collect: string, flag: number) => void): void {
    this._escHandlerFb = callback;
  }

  addOscHandler(ident: number, callback: (data: string) => boolean): IDisposable {
    if (this._oscHandlers[ident] === undefined) {
      this._oscHandlers[ident] = [];
    }
    const handlerList = this._oscHandlers[ident];
    handlerList.push(callback);
    return {
      dispose: () => {
        const handlerIndex = handlerList.indexOf(callback);
        if (handlerIndex !== -1) {
          handlerList.splice(handlerIndex, 1);
        }
      }
    };
  }
  setOscHandler(ident: number, callback: (data: string) => void): void {
    this._oscHandlers[ident] = [callback];
  }
  clearOscHandler(ident: number): void {
    if (this._oscHandlers[ident]) delete this._oscHandlers[ident];
  }
  setOscHandlerFallback(callback: (identifier: number, data: string) => void): void {
    this._oscHandlerFb = callback;
  }

  setDcsHandler(collectAndFlag: string, handler: IDcsHandler): void {
    this._dcsHandlers[collectAndFlag] = handler;
  }
  clearDcsHandler(collectAndFlag: string): void {
    if (this._dcsHandlers[collectAndFlag]) delete this._dcsHandlers[collectAndFlag];
  }
  setDcsHandlerFallback(handler: IDcsHandler): void {
    this._dcsHandlerFb = handler;
  }

  setErrorHandler(callback: (state: IParsingState) => IParsingState): void {
    this._errorHandler = callback;
  }
  clearErrorHandler(): void {
    this._errorHandler = this._errorHandlerFb;
  }

  reset(): void {
    this.currentState = this.initialState;
    this._osc = '';
    this._params = [0];
    this._collect = '';
    this._activeDcsHandler = null;
  }

  parse(data: Uint32Array, length: number): void {
    let code = 0;
    let transition = 0;
    let error = false;
    let currentState = this.currentState;
    let print = -1;
    let dcs = -1;
    let osc = this._osc;
    let collect = this._collect;
    let params = this._params;
    const table: Uint8Array | number[] = this.TRANSITIONS.table;
    let dcsHandler: IDcsHandler | null = this._activeDcsHandler;
    let callback: Function | null = null;

    // process input string
    for (let i = 0; i < length; ++i) {
      code = data[i];

      // shortcut for most chars (print action)
      if (currentState === ParserState.GROUND && code > 0x1f && code < 0x80) {
        print = (~print) ? print : i;
        do i++;
        while (i < length && data[i] > 0x1f && data[i] < 0x80);
        i--;
        continue;
      }

      // shortcut for CSI params
      if (currentState === ParserState.CSI_PARAM && (code > 0x2f && code < 0x39)) {
        params[params.length - 1] = params[params.length - 1] * 10 + code - 48;
        continue;
      }

      // normal transition & action lookup
      transition = table[currentState << 8 | (code < 0xa0 ? code : NON_ASCII_PRINTABLE)];
      switch (transition >> 4) {
        case ParserAction.PRINT:
          print = (~print) ? print : i;
          break;
        case ParserAction.EXECUTE:
          if (~print) {
            this._printHandler(data, print, i);
            print = -1;
          }
          callback = this._executeHandlers[code];
          if (callback) callback();
          else this._executeHandlerFb(code);
          break;
        case ParserAction.IGNORE:
          // handle leftover print or dcs chars
          if (~print) {
            this._printHandler(data, print, i);
            print = -1;
          } else if (~dcs) {
            dcsHandler.put(data, dcs, i);
            dcs = -1;
          }
          break;
        case ParserAction.ERROR:
          // chars higher than 0x9f are handled by this action
          // to keep the transition table small
          if (code > 0x9f) {
            switch (currentState) {
              case ParserState.GROUND:
                print = (~print) ? print : i;
                break;
              case ParserState.CSI_IGNORE:
                transition |= ParserState.CSI_IGNORE;
                break;
              case ParserState.DCS_IGNORE:
                transition |= ParserState.DCS_IGNORE;
                break;
              case ParserState.DCS_PASSTHROUGH:
                dcs = (~dcs) ? dcs : i;
                transition |= ParserState.DCS_PASSTHROUGH;
                break;
              default:
                error = true;
            }
          } else {
            error = true;
          }
          // if we end up here a real error happened
          if (error) {
            const inject: IParsingState = this._errorHandler(
              {
                position: i,
                code,
                currentState,
                print,
                dcs,
                osc,
                collect,
                params,
                abort: false
              });
            if (inject.abort) return;
          // TODO: inject return values
            error = false;
          }
          break;
        case ParserAction.CSI_DISPATCH:
          // Trigger CSI Handler
          const handlers = this._csiHandlers[code];
          let j = handlers ? handlers.length - 1 : -1;
          for (; j >= 0; j--) {
            // undefined or true means success and to stop bubbling
            if (handlers[j](params, collect) !== false) {
              break;
            }
          }
          if (j < 0) {
            this._csiHandlerFb(collect, params, code);
          }
          break;
        case ParserAction.PARAM:
          if (code === 0x3b) params.push(0);
          else params[params.length - 1] = params[params.length - 1] * 10 + code - 48;
          break;
        case ParserAction.COLLECT:
          collect += String.fromCharCode(code);
          break;
        case ParserAction.ESC_DISPATCH:
          callback = this._escHandlers[collect + String.fromCharCode(code)];
          if (callback) callback(collect, code);
          else this._escHandlerFb(collect, code);
          break;
        case ParserAction.CLEAR:
          if (~print) {
            this._printHandler(data, print, i);
            print = -1;
          }
          osc = '';
          params = [0];
          collect = '';
          dcs = -1;
          break;
        case ParserAction.DCS_HOOK:
          dcsHandler = this._dcsHandlers[collect + String.fromCharCode(code)];
          if (!dcsHandler) dcsHandler = this._dcsHandlerFb;
          dcsHandler.hook(collect, params, code);
          break;
        case ParserAction.DCS_PUT:
          dcs = (~dcs) ? dcs : i;
          break;
        case ParserAction.DCS_UNHOOK:
          if (dcsHandler) {
            if (~dcs) dcsHandler.put(data, dcs, i);
            dcsHandler.unhook();
            dcsHandler = null;
          }
          if (code === 0x1b) transition |= ParserState.ESCAPE;
          osc = '';
          params = [0];
          collect = '';
          dcs = -1;
          break;
        case ParserAction.OSC_START:
          if (~print) {
            this._printHandler(data, print, i);
            print = -1;
          }
          osc = '';
          break;
        case ParserAction.OSC_PUT:
          for (let j = i + 1; ; j++) {
            if (j >= length
                || (code = data[j]) < 0x20
                || (code > 0x7f && code <= 0x9f)) {
              osc += utf32ToString(data, i, j);
              i = j - 1;
              break;
            }
          }
          break;
        case ParserAction.OSC_END:
          if (osc && code !== 0x18 && code !== 0x1a) {
            // NOTE: OSC subparsing is not part of the original parser
            // we do basic identifier parsing here to offer a jump table for OSC as well
            const idx = osc.indexOf(';');
            if (idx === -1) {
              this._oscHandlerFb(-1, osc);  // this is an error (malformed OSC)
            } else {
              // Note: NaN is not handled here
              // either catch it with the fallback handler
              // or with an explicit NaN OSC handler
              const identifier = parseInt(osc.substring(0, idx));
              const content = osc.substring(idx + 1);
              // Trigger OSC Handler
              const handlers = this._oscHandlers[identifier];
              let j = handlers ? handlers.length - 1 : -1;
              for (; j >= 0; j--) {
                // undefined or true means success and to stop bubbling
                if (handlers[j](content) !== false) {
                  break;
                }
              }
              if (j < 0) {
                this._oscHandlerFb(identifier, content);
              }
            }
          }
          if (code === 0x1b) transition |= ParserState.ESCAPE;
          osc = '';
          params = [0];
          collect = '';
          dcs = -1;
          break;
      }
      currentState = transition & 15;
    }

    // push leftover pushable buffers to terminal
    if (currentState === ParserState.GROUND && ~print) {
      this._printHandler(data, print, length);
    } else if (currentState === ParserState.DCS_PASSTHROUGH && ~dcs && dcsHandler) {
      dcsHandler.put(data, dcs, length);
    }

    // save non pushable buffers
    this._osc = osc;
    this._collect = collect;
    this._params = params;

    // save active dcs handler reference
    this._activeDcsHandler = dcsHandler;

    // save state
    this.currentState = currentState;
  }
}
