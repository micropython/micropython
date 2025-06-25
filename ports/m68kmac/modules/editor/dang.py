# SPDX-FileCopyrightText: 2023 Jeff Epler for Adafruit Industries
#
# SPDX-License-Identifier: MIT

try:
    import select
except:
    select = None

import sys
# pylint: disable=no-self-use

try:
    import termios

    _orig_attr = None  # pylint: disable=invalid-name

    def _nonblocking():
        global _orig_attr  # pylint: disable=global-statement
        _orig_attr = termios.tcgetattr(sys.stdin)
        attr = termios.tcgetattr(sys.stdin)
        attr[3] &= ~(termios.ECHO | termios.ICANON)
        attr[6][termios.VMIN] = 1
        attr[6][termios.VTIME] = 0
        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, attr)

    def _blocking():
        if _orig_attr is not None:
            termios.tcsetattr(sys.stdin, termios.TCSADRAIN, _orig_attr)

except ImportError:

    def _nonblocking():
        pass

    def _blocking():
        pass


LINES = 24
COLS = 80


def CTRL(c):
    return chr(ord(c) & 0x1F)


special_keys = {
    # Some emacs style bindings for Mac w/o arrow keys
    CTRL('a'): "KEY_HOME",
    CTRL('e'): "KEY_END",
    CTRL('p'): "KEY_UP",
    CTRL('n'): "KEY_DOWN",
    CTRL('b'): "KEY_LEFT",
    CTRL('f'): "KEY_RIGHT",
    CTRL('v'): "KEY_PGDN",
    "\x1bv": "KEY_PGUP",
    CTRL('d'): "KEY_DELETE",
    "\x1b": ...,  # all prefixes of special keys must be entered as Ellipsis
    "\x1b[": ...,
    "\x1b[5": ...,
    "\x1b[6": ...,
    "\x1b[A": "KEY_UP",
    "\x1b[B": "KEY_DOWN",
    "\x1b[C": "KEY_RIGHT",
    "\x1b[D": "KEY_LEFT",
    "\x1b[H": "KEY_HOME",
    "\x1b[F": "KEY_END",
    "\x1b[5~": "KEY_PGUP",
    "\x1b[6~": "KEY_PGDN",
    "\x1b[3~": "KEY_DELETE",
}


class FakePoll:
    def poll(self, timeout):
        return True


class Screen:
    def __init__(self):
        if select is None:
            self._poll = select.poll()
            self._poll.register(sys.stdin, select.POLLIN)
        else:
            self._poll = FakePoll()
        self._pending = ""

    def _sys_stdin_readable(self):
        return hasattr(sys.stdin, "readable") and sys.stdin.readable()

    def _sys_stdout_flush(self):
        if hasattr(sys, 'stdout') and hasattr(sys.stdout, "flush"):
            sys.stdout.flush()

    def _terminal_read_blocking(self):
        return sys.stdin.read(1)

    def _terminal_read_timeout(self, timeout):
        if self._sys_stdin_readable() or self._poll.poll(timeout):
            r = sys.stdin.read(1)
            return r
        return None

    def move(self, y, x):
        print(end=f"\033[{y + 1};{x + 1}H")

    def erase(self):
        print(end="\033H\033[2J")

    def addstr(self, y, x, text):
        self.move(y, x)
        print(end=text)

    def getkey(self):
        self._sys_stdout_flush()
        pending = self._pending
        if pending and (code := special_keys.get(pending)) is None:
            self._pending = pending[1:]
            return pending[0]

        while True:
            if pending:
                c = self._terminal_read_timeout(50)
                if c is None:
                    self._pending = pending[1:]
                    return pending[0]
            else:
                c = self._terminal_read_blocking()
            c = pending + c

            code = special_keys.get(c)

            if code is None:
                self._pending = c[1:]
                return c[0]
            if code is not Ellipsis:
                return code

            pending = c


def wrapper(func, *args, **kwds):
    stdscr = Screen()
    try:
        _nonblocking()
        return func(stdscr, *args, **kwds)
    finally:
        _blocking()
        stdscr.move(LINES - 1, 0)
        print("\n")
