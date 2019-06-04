/**
 * Copyright (c) 2014 The xterm.js authors. All rights reserved.
 * Copyright (c) 2012-2013, Christopher Jeffrey (MIT License)
 * @license MIT
 */

import { IKeyboardEvent } from '../../common/Types';
import { IKeyboardResult, KeyboardResultType } from '../Types';
import { C0 } from '../../common/data/EscapeSequences';

// reg + shift key mappings for digits and special chars
const KEYCODE_KEY_MAPPINGS: { [key: number]: [string, string]} = {
  // digits 0-9
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

  // special chars
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

export function evaluateKeyboardEvent(
  ev: IKeyboardEvent,
  applicationCursorMode: boolean,
  isMac: boolean,
  macOptionIsMeta: boolean
): IKeyboardResult {
  const result: IKeyboardResult = {
    type: KeyboardResultType.SEND_KEY,
    // Whether to cancel event propagation (NOTE: this may not be needed since the event is
    // canceled at the end of keyDown
    cancel: false,
    // The new key even to emit
    key: undefined
  };
  const modifiers = (ev.shiftKey ? 1 : 0) | (ev.altKey ? 2 : 0) | (ev.ctrlKey ? 4 : 0) | (ev.metaKey ? 8 : 0);
  switch (ev.keyCode) {
    case 0:
      if (ev.key === 'UIKeyInputUpArrow') {
        if (applicationCursorMode) {
          result.key = C0.ESC + 'OA';
        } else {
          result.key = C0.ESC + '[A';
        }
      }
      else if (ev.key === 'UIKeyInputLeftArrow') {
        if (applicationCursorMode) {
          result.key = C0.ESC + 'OD';
        } else {
          result.key = C0.ESC + '[D';
        }
      }
      else if (ev.key === 'UIKeyInputRightArrow') {
        if (applicationCursorMode) {
          result.key = C0.ESC + 'OC';
        } else {
          result.key = C0.ESC + '[C';
        }
      }
      else if (ev.key === 'UIKeyInputDownArrow') {
        if (applicationCursorMode) {
          result.key = C0.ESC + 'OB';
        } else {
          result.key = C0.ESC + '[B';
        }
      }
      break;
    case 8:
      // backspace
      if (ev.shiftKey) {
        result.key = C0.BS; // ^H
        break;
      } else if (ev.altKey) {
        result.key = C0.ESC + C0.DEL; // \e ^?
        break;
      }
      result.key = C0.DEL; // ^?
      break;
    case 9:
      // tab
      if (ev.shiftKey) {
        result.key = C0.ESC + '[Z';
        break;
      }
      result.key = C0.HT;
      result.cancel = true;
      break;
    case 13:
      // return/enter
      result.key = C0.CR;
      result.cancel = true;
      break;
    case 27:
      // escape
      result.key = C0.ESC;
      result.cancel = true;
      break;
    case 37:
      // left-arrow
      if (modifiers) {
        result.key = C0.ESC + '[1;' + (modifiers + 1) + 'D';
        // HACK: Make Alt + left-arrow behave like Ctrl + left-arrow: move one word backwards
        // http://unix.stackexchange.com/a/108106
        // macOS uses different escape sequences than linux
        if (result.key === C0.ESC + '[1;3D') {
          result.key = isMac ? C0.ESC + 'b' : C0.ESC + '[1;5D';
        }
      } else if (applicationCursorMode) {
        result.key = C0.ESC + 'OD';
      } else {
        result.key = C0.ESC + '[D';
      }
      break;
    case 39:
      // right-arrow
      if (modifiers) {
        result.key = C0.ESC + '[1;' + (modifiers + 1) + 'C';
        // HACK: Make Alt + right-arrow behave like Ctrl + right-arrow: move one word forward
        // http://unix.stackexchange.com/a/108106
        // macOS uses different escape sequences than linux
        if (result.key === C0.ESC + '[1;3C') {
          result.key = isMac ? C0.ESC + 'f' : C0.ESC + '[1;5C';
        }
      } else if (applicationCursorMode) {
        result.key = C0.ESC + 'OC';
      } else {
        result.key = C0.ESC + '[C';
      }
      break;
    case 38:
      // up-arrow
      if (modifiers) {
        result.key = C0.ESC + '[1;' + (modifiers + 1) + 'A';
        // HACK: Make Alt + up-arrow behave like Ctrl + up-arrow
        // http://unix.stackexchange.com/a/108106
        if (result.key === C0.ESC + '[1;3A') {
          result.key = C0.ESC + '[1;5A';
        }
      } else if (applicationCursorMode) {
        result.key = C0.ESC + 'OA';
      } else {
        result.key = C0.ESC + '[A';
      }
      break;
    case 40:
      // down-arrow
      if (modifiers) {
        result.key = C0.ESC + '[1;' + (modifiers + 1) + 'B';
        // HACK: Make Alt + down-arrow behave like Ctrl + down-arrow
        // http://unix.stackexchange.com/a/108106
        if (result.key === C0.ESC + '[1;3B') {
          result.key = C0.ESC + '[1;5B';
        }
      } else if (applicationCursorMode) {
        result.key = C0.ESC + 'OB';
      } else {
        result.key = C0.ESC + '[B';
      }
      break;
    case 45:
      // insert
      if (!ev.shiftKey && !ev.ctrlKey) {
        // <Ctrl> or <Shift> + <Insert> are used to
        // copy-paste on some systems.
        result.key = C0.ESC + '[2~';
      }
      break;
    case 46:
      // delete
      if (modifiers) {
        result.key = C0.ESC + '[3;' + (modifiers + 1) + '~';
      } else {
        result.key = C0.ESC + '[3~';
      }
      break;
    case 36:
      // home
      if (modifiers) {
        result.key = C0.ESC + '[1;' + (modifiers + 1) + 'H';
      } else if (applicationCursorMode) {
        result.key = C0.ESC + 'OH';
      } else {
        result.key = C0.ESC + '[H';
      }
      break;
    case 35:
      // end
      if (modifiers) {
        result.key = C0.ESC + '[1;' + (modifiers + 1) + 'F';
      } else if (applicationCursorMode) {
        result.key = C0.ESC + 'OF';
      } else {
        result.key = C0.ESC + '[F';
      }
      break;
    case 33:
      // page up
      if (ev.shiftKey) {
        result.type = KeyboardResultType.PAGE_UP;
      } else {
        result.key = C0.ESC + '[5~';
      }
      break;
    case 34:
      // page down
      if (ev.shiftKey) {
        result.type = KeyboardResultType.PAGE_DOWN;
      } else {
        result.key = C0.ESC + '[6~';
      }
      break;
    case 112:
      // F1-F12
      if (modifiers) {
        result.key = C0.ESC + '[1;' + (modifiers + 1) + 'P';
      } else {
        result.key = C0.ESC + 'OP';
      }
      break;
    case 113:
      if (modifiers) {
        result.key = C0.ESC + '[1;' + (modifiers + 1) + 'Q';
      } else {
        result.key = C0.ESC + 'OQ';
      }
      break;
    case 114:
      if (modifiers) {
        result.key = C0.ESC + '[1;' + (modifiers + 1) + 'R';
      } else {
        result.key = C0.ESC + 'OR';
      }
      break;
    case 115:
      if (modifiers) {
        result.key = C0.ESC + '[1;' + (modifiers + 1) + 'S';
      } else {
        result.key = C0.ESC + 'OS';
      }
      break;
    case 116:
      if (modifiers) {
        result.key = C0.ESC + '[15;' + (modifiers + 1) + '~';
      } else {
        result.key = C0.ESC + '[15~';
      }
      break;
    case 117:
      if (modifiers) {
        result.key = C0.ESC + '[17;' + (modifiers + 1) + '~';
      } else {
        result.key = C0.ESC + '[17~';
      }
      break;
    case 118:
      if (modifiers) {
        result.key = C0.ESC + '[18;' + (modifiers + 1) + '~';
      } else {
        result.key = C0.ESC + '[18~';
      }
      break;
    case 119:
      if (modifiers) {
        result.key = C0.ESC + '[19;' + (modifiers + 1) + '~';
      } else {
        result.key = C0.ESC + '[19~';
      }
      break;
    case 120:
      if (modifiers) {
        result.key = C0.ESC + '[20;' + (modifiers + 1) + '~';
      } else {
        result.key = C0.ESC + '[20~';
      }
      break;
    case 121:
      if (modifiers) {
        result.key = C0.ESC + '[21;' + (modifiers + 1) + '~';
      } else {
        result.key = C0.ESC + '[21~';
      }
      break;
    case 122:
      if (modifiers) {
        result.key = C0.ESC + '[23;' + (modifiers + 1) + '~';
      } else {
        result.key = C0.ESC + '[23~';
      }
      break;
    case 123:
      if (modifiers) {
        result.key = C0.ESC + '[24;' + (modifiers + 1) + '~';
      } else {
        result.key = C0.ESC + '[24~';
      }
      break;
    default:
      // a-z and space
      if (ev.ctrlKey && !ev.shiftKey && !ev.altKey && !ev.metaKey) {
        if (ev.keyCode >= 65 && ev.keyCode <= 90) {
          result.key = String.fromCharCode(ev.keyCode - 64);
        } else if (ev.keyCode === 32) {
          // NUL
          result.key = String.fromCharCode(0);
        } else if (ev.keyCode >= 51 && ev.keyCode <= 55) {
          // escape, file sep, group sep, record sep, unit sep
          result.key = String.fromCharCode(ev.keyCode - 51 + 27);
        } else if (ev.keyCode === 56) {
          // delete
          result.key = String.fromCharCode(127);
        } else if (ev.keyCode === 219) {
          // ^[ - Control Sequence Introducer (CSI)
          result.key = String.fromCharCode(27);
        } else if (ev.keyCode === 220) {
          // ^\ - String Terminator (ST)
          result.key = String.fromCharCode(28);
        } else if (ev.keyCode === 221) {
          // ^] - Operating System Command (OSC)
          result.key = String.fromCharCode(29);
        }
      } else if ((!isMac || macOptionIsMeta) && ev.altKey && !ev.metaKey) {
        // On macOS this is a third level shift when !macOptionIsMeta. Use <Esc> instead.
        const keyMapping = KEYCODE_KEY_MAPPINGS[ev.keyCode];
        const key = keyMapping && keyMapping[!ev.shiftKey ? 0 : 1];
        if (key) {
          result.key = C0.ESC + key;
        } else if (ev.keyCode >= 65 && ev.keyCode <= 90) {
          const keyCode = ev.ctrlKey ? ev.keyCode - 64 : ev.keyCode + 32;
          result.key = C0.ESC + String.fromCharCode(keyCode);
        }
      } else if (isMac && !ev.altKey && !ev.ctrlKey && ev.metaKey) {
        if (ev.keyCode === 65) { // cmd + a
          result.type = KeyboardResultType.SELECT_ALL;
        }
      } else if (ev.key && !ev.ctrlKey && !ev.altKey && !ev.metaKey && ev.keyCode >= 48 && ev.key.length === 1) {
        // Include only keys that that result in a _single_ character; don't include num lock, volume up, etc.
        result.key = ev.key;
      } else if (ev.key && ev.ctrlKey) {
        if (ev.key === '_') { // ^_
          result.key = C0.US;
        }
      }
      break;
  }

  return result;
}
