# SPDX-FileCopyrightText: 2020 Wasim Lorgat
# SPDX-FileCopyrightText: 2023 Jeff Epler for Adafruit Industries
# SPDX-FileCopyrightText: 2024 Tim Cocks for Adafruit Industries
#
# SPDX-License-Identifier: MIT

import gc
import os
from . import dang as curses
# pylint: disable=redefined-builtin


class MaybeDisableReload:
    def __enter__(self):
        try:
            from supervisor import runtime  # pylint: disable=import-outside-toplevel
        except ImportError:
            return

        self._old_autoreload = (  # pylint: disable=attribute-defined-outside-init
            runtime.autoreload
        )
        runtime.autoreload = False

    def __exit__(self, exc_type, exc_value, traceback):
        try:
            from supervisor import runtime  # pylint: disable=import-outside-toplevel
        except ImportError:
            return

        runtime.autoreload = self._old_autoreload


def os_exists(filename):
    try:
        with open(filename, "r"):
            return True
    except OSError:
        return False


def gc_mem_free_hint():
    if hasattr(gc, "mem_free"):
        gc.collect()
        return f" | free: {gc.mem_free()}"
    return ""


class Buffer:
    def __init__(self, lines):
        self.lines = lines

    def __len__(self):
        return len(self.lines)

    def __getitem__(self, index):
        return self.lines[index]

    @property
    def bottom(self):
        return len(self) - 1

    def insert(self, cursor, string):
        row, col = cursor.row, cursor.col
        try:
            current = self.lines.pop(row)
        except IndexError:
            current = ""
        new = current[:col] + string + current[col:]
        self.lines.insert(row, new)

    def split(self, cursor):
        row, col = cursor.row, cursor.col
        current = self.lines.pop(row)
        self.lines.insert(row, current[:col])
        self.lines.insert(row + 1, current[col:])

    def delete(self, cursor):
        row, col = cursor.row, cursor.col
        if (row, col) < (self.bottom, len(self[row])):
            current = self.lines.pop(row)
            if col < len(current):
                new = current[:col] + current[col + 1 :]
                self.lines.insert(row, new)
            else:
                nextline = self.lines.pop(row)
                new = current + nextline
                self.lines.insert(row, new)


def clamp(x, lower, upper):
    if x < lower:
        return lower
    if x > upper:
        return upper
    return x


class Cursor:
    def __init__(self, row=0, col=0, col_hint=None):
        self.row = row
        self._col = col
        self._col_hint = col if col_hint is None else col_hint

    @property
    def col(self):
        return self._col

    @col.setter
    def col(self, col):
        self._col = col
        self._col_hint = col

    def _clamp_col(self, buffer):
        self._col = min(self._col_hint, len(buffer[self.row]))

    def up(self, buffer):  # pylint: disable=invalid-name
        if self.row > 0:
            self.row -= 1
            self._clamp_col(buffer)

    def down(self, buffer):
        if self.row < len(buffer) - 1:
            self.row += 1
            self._clamp_col(buffer)

    def left(self, buffer):
        if self.col > 0:
            self.col -= 1
        elif self.row > 0:
            self.row -= 1
            self.col = len(buffer[self.row])

    def right(self, buffer):
        if len(buffer) > 0 and self.col < len(buffer[self.row]):
            self.col += 1
        elif self.row < len(buffer) - 1:
            self.row += 1
            self.col = 0

    def end(self, buffer):
        self.col = len(buffer[self.row])


class Window:
    def __init__(self, n_rows, n_cols, row=0, col=0):
        self.n_rows = n_rows
        self.n_cols = n_cols
        self.row = row
        self.col = col

    @property
    def bottom(self):
        return self.row + self.n_rows - 1

    def up(self, cursor):  # pylint: disable=invalid-name
        if cursor.row == self.row - 1 and self.row > 0:
            self.row -= 1

    def down(self, buffer, cursor):
        if cursor.row == self.bottom + 1 and self.bottom < len(buffer) - 1:
            self.row += 1

    def horizontal_scroll(self, cursor, left_margin=5, right_margin=2):
        n_pages = cursor.col // (self.n_cols - right_margin)
        self.col = max(n_pages * self.n_cols - right_margin - left_margin, 0)

    def translate(self, cursor):
        return cursor.row - self.row, cursor.col - self.col


def left(window, buffer, cursor):
    cursor.left(buffer)
    window.up(cursor)
    window.horizontal_scroll(cursor)


def right(window, buffer, cursor):
    cursor.right(buffer)
    window.down(buffer, cursor)
    window.horizontal_scroll(cursor)


def home(window, buffer, cursor):  # pylint: disable=unused-argument
    cursor.col = 0
    window.horizontal_scroll(cursor)


def end(window, buffer, cursor):
    cursor.end(buffer)
    window.horizontal_scroll(cursor)


def editor(stdscr, filename):  # pylint: disable=too-many-branches,too-many-statements
    if os_exists(filename):
        with open(filename, "r", encoding="utf-8") as f:
            buffer = Buffer(f.read().splitlines())
    else:
        buffer = Buffer([""])

    window = Window(curses.LINES - 1, curses.COLS - 1)
    cursor = Cursor()

    stdscr.erase()

    img = [None] * curses.LINES

    def setline(row, line):
        if img[row] == line:
            return
        img[row] = line
        line += " " * (window.n_cols - len(line))
        stdscr.addstr(row, 0, line)

    while True:
        lastrow = 0
        for row, line in enumerate(buffer[window.row : window.row + window.n_rows]):
            lastrow = row
            if row == cursor.row - window.row and window.col > 0:
                line = "«" + line[window.col + 1 :]
            if len(line) > window.n_cols:
                line = line[: window.n_cols - 1] + "»"
            setline(row, line)
        for row in range(lastrow + 1, window.n_rows):
            setline(row, "~~ EOF ~~")
        row = curses.LINES - 1
        line = f"{filename:12} | ^X: write & exit | ^C: quit w/o save{gc_mem_free_hint()}"
        setline(row, line)

        stdscr.move(*window.translate(cursor))

        k = stdscr.getkey()
        if len(k) == 1 and " " <= k <= "~":
            buffer.insert(cursor, k)
            for _ in k:
                right(window, buffer, cursor)
        elif k == "\x18":  # ctrl-x
            with open(filename, "w", encoding="utf-8") as f:
                for row in buffer:
                    f.write(f"{row}\n")
            return
        elif k == "\x11":  # Ctrl-Q
            for row in buffer:
                print(row)
        elif k == "KEY_HOME":
            home(window, buffer, cursor)
        elif k == "KEY_END":
            end(window, buffer, cursor)
        elif k == "KEY_LEFT":
            left(window, buffer, cursor)
        elif k == "KEY_DOWN":
            cursor.down(buffer)
            window.down(buffer, cursor)
            window.horizontal_scroll(cursor)
        elif k == "KEY_PGDN":
            for _ in range(window.n_rows):
                cursor.down(buffer)
                window.down(buffer, cursor)
                window.horizontal_scroll(cursor)
        elif k == "KEY_UP":
            cursor.up(buffer)
            window.up(cursor)
            window.horizontal_scroll(cursor)
        elif k == "KEY_PGUP":
            for _ in range(window.n_rows):
                cursor.up(buffer)
                window.up(cursor)
                window.horizontal_scroll(cursor)
        elif k == "KEY_RIGHT":
            right(window, buffer, cursor)
        elif k == "\n" or k == "\r":
            buffer.split(cursor)
            right(window, buffer, cursor)
        elif k in ("KEY_DELETE", "\x04"):
            buffer.delete(cursor)

        elif k in ("KEY_BACKSPACE", "\x7f", "\x08"):
            if (cursor.row, cursor.col) > (0, 0):
                left(window, buffer, cursor)
                buffer.delete(cursor)


def edit(filename):
    return curses.wrapper(editor, filename)
