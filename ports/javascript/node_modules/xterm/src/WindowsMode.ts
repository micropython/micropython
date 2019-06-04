/**
 * Copyright (c) 2019 The xterm.js authors. All rights reserved.
 * @license MIT
 */

import { IDisposable } from 'xterm';
import { ITerminal } from './Types';
import { CHAR_DATA_CODE_INDEX, NULL_CELL_CODE, WHITESPACE_CELL_CODE } from './core/buffer/BufferLine';

export function applyWindowsMode(terminal: ITerminal): IDisposable {
  // Winpty does not support wraparound mode which means that lines will never
  // be marked as wrapped. This causes issues for things like copying a line
  // retaining the wrapped new line characters or if consumers are listening
  // in on the data stream.
  //
  // The workaround for this is to listen to every incoming line feed and mark
  // the line as wrapped if the last character in the previous line is not a
  // space. This is certainly not without its problems, but generally on
  // Windows when text reaches the end of the terminal it's likely going to be
  // wrapped.
  return terminal.onLineFeed(() => {
    const line = terminal.buffer.lines.get(terminal.buffer.ybase + terminal.buffer.y - 1);
    const lastChar = line.get(terminal.cols - 1);

    if (lastChar[CHAR_DATA_CODE_INDEX] !== NULL_CELL_CODE && lastChar[CHAR_DATA_CODE_INDEX] !== WHITESPACE_CELL_CODE) {
      const nextLine = terminal.buffer.lines.get(terminal.buffer.ybase + terminal.buffer.y);
      nextLine.isWrapped = true;
    }
  });
}
