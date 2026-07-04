# This file is part of the MicroPython project, http://micropython.org/
#
# The MIT License (MIT)
#
# Copyright (c) 2026 Nicko van Someren
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

# uefi.console — text console control over ConOut / ConIn.
#
# The interactive half of the shell: clear/colour/cursor/mode on the active text
# output (ConOut), and raw keystroke input incl. scancodes (arrow keys, F-keys)
# on the active text input (ConIn). This is what a boot-menu UI is built from.
#
# ConOut/ConIn are the *active* console interfaces named in the System Table
# (not just any text handle), so we read their pointers from there and wrap them.

import uctypes

from . import raw
from . import status
from .protocol import ProtocolDescriptor, Field, Method, VPtr, U64, Bool, WStr, I32

# EFI text colours: foreground 0..15, background 0..7 (background high-intensity
# bit is reserved). Attribute = fg | (bg << 4).
BLACK = 0x0
BLUE = 0x1
GREEN = 0x2
CYAN = 0x3
RED = 0x4
MAGENTA = 0x5
BROWN = 0x6
LIGHTGRAY = 0x7
DARKGRAY = 0x8
LIGHTBLUE = 0x9
LIGHTGREEN = 0xA
LIGHTCYAN = 0xB
LIGHTRED = 0xC
LIGHTMAGENTA = 0xD
YELLOW = 0xE
WHITE = 0xF


def attribute(fg, bg=BLACK):
    """Pack a foreground (0..15) and background (0..7) into an EFI text attribute."""
    return (fg & 0x0F) | ((bg & 0x07) << 4)


# EFI_INPUT_KEY.ScanCode values (UnicodeChar is 0 for these).
SCAN_NULL = 0x00
SCAN_UP = 0x01
SCAN_DOWN = 0x02
SCAN_RIGHT = 0x03
SCAN_LEFT = 0x04
SCAN_HOME = 0x05
SCAN_END = 0x06
SCAN_INSERT = 0x07
SCAN_DELETE = 0x08
SCAN_PAGE_UP = 0x09
SCAN_PAGE_DOWN = 0x0A
SCAN_F1 = 0x0B
SCAN_F2 = 0x0C
SCAN_F3 = 0x0D
SCAN_F4 = 0x0E
SCAN_ESC = 0x17

# EFI_SIMPLE_TEXT_OUTPUT_MODE (all INT32 + a trailing BOOLEAN).
TEXT_OUTPUT_MODE = ProtocolDescriptor(
    "EFI_SIMPLE_TEXT_OUTPUT_MODE",
    None,
    [
        Field("max_mode", I32),
        Field("mode", I32),
        Field("attribute", I32),
        Field("cursor_column", I32),
        Field("cursor_row", I32),
        Field("cursor_visible", Bool),
    ],
)

TEXT_OUTPUT = ProtocolDescriptor(
    "EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL",
    None,
    [
        Method("reset", (Bool,)),
        Method("output_string", (WStr,), check=False),
        Method("test_string", (WStr,), check=False),
        Method("query_mode", (U64, VPtr, VPtr)),  # (ModeNumber, *Columns, *Rows)
        Method("set_mode", (U64,)),  # (ModeNumber)
        Method("set_attribute", (U64,)),  # (Attribute)
        Method("clear_screen", ()),
        Method("set_cursor_position", (U64, U64)),  # (Column, Row)
        Method("enable_cursor", (Bool,)),
        Field("mode", VPtr),  # SIMPLE_TEXT_OUTPUT_MODE *
    ],
)

TEXT_INPUT = ProtocolDescriptor(
    "EFI_SIMPLE_TEXT_INPUT_PROTOCOL",
    None,
    [
        Method("reset", (Bool,)),
        Method("read_key_stroke", (VPtr,), check=False),  # (*EFI_INPUT_KEY); NOT_READY if none
        Field("wait_for_key", VPtr),  # EFI_EVENT
    ],
)

# System Table offsets (both targets are LP64): ConIn @48, ConOut @64.
_ST_CONSOLE = {
    "conin": uctypes.UINT64 | 48,
    "conout": uctypes.UINT64 | 64,
}


class Console:
    """The active text console: ConOut for output, ConIn for keystrokes."""

    def __init__(self):
        st = uctypes.struct(raw.system_table_ptr(), _ST_CONSOLE, uctypes.LITTLE_ENDIAN)
        self.out = TEXT_OUTPUT.wrap(st.conout)
        self.inp = TEXT_INPUT.wrap(st.conin)

    # ── output ───────────────────────────────────────────────────────────────
    def write(self, text):
        self.out.output_string(text)

    def clear(self):
        self.out.clear_screen()

    def set_color(self, fg, bg=BLACK):
        self.out.set_attribute(attribute(fg, bg))

    def set_cursor(self, column, row):
        self.out.set_cursor_position(column, row)

    def show_cursor(self, visible=True):
        self.out.enable_cursor(bool(visible))

    def _mode(self):
        return self.out.struct_at("mode", TEXT_OUTPUT_MODE)

    def cursor(self):
        """(column, row) of the text cursor."""
        m = self._mode()
        return m.cursor_column, m.cursor_row

    def mode_count(self):
        """Number of text modes the console supports."""
        return self._mode().max_mode

    def current_mode(self):
        return self._mode().mode

    def set_mode(self, n):
        self.out.set_mode(n)

    def size(self, mode=None):
        """(columns, rows) of `mode` (default: the current mode)."""
        n = self.current_mode() if mode is None else mode
        cols = bytearray(8)
        rows = bytearray(8)
        self.out.query_mode(n, cols, rows)
        return int.from_bytes(cols, "little"), int.from_bytes(rows, "little")

    # ── input ────────────────────────────────────────────────────────────────
    def read_key(self):
        """Non-blocking: return (scancode, char) or None if no key is pending.

        `char` is a 1-char str (or '' for a pure scancode key like an arrow).
        """
        key = bytearray(4)
        st = self.inp.read_key_stroke(key)
        if st == status.NOT_READY:
            return None
        status.check(st)
        scan = key[0] | (key[1] << 8)
        char = key[2] | (key[3] << 8)
        return (scan, chr(char) if char else "")

    def wait_key(self):
        """Block until a key is pressed; return (scancode, char).

        Uses BS->WaitForEvent on the ConIn WaitForKey event — a hard block at
        TPL_APPLICATION (it does not yield to asyncio), which is what a modal boot
        menu wants.
        """
        ev = self.inp.wait_for_key
        while True:
            raw.wait_for_event([ev])
            k = self.read_key()
            if k is not None:
                return k

    def flush_input(self):
        """Discard any pending keystrokes."""
        while self.read_key() is not None:
            pass
