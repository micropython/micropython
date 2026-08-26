# SPDX-License-Identifier: MIT
# Copyright (c) 2026 Nicko van Someren

# uefi.console — text console control over ConOut / ConIn.
#
# The interactive half of the shell: clear/colour/cursor/mode on the active text
# output (ConOut), and raw keystroke input incl. scancodes (arrow keys, F-keys)
# on the active text input (ConIn). This is what a boot-menu UI is built from.
#
# ConOut/ConIn are the *active* console interfaces named in the System Table
# (not just any text handle), so we read their pointers from there and wrap them.
#
# Keystrokes arrive three ways: read_key() polls, wait_key() blocks in the
# firmware's WaitForEvent, and key_event() hands back ConIn's WaitForKey as a
# pollable uefi.Event for select / asyncio (wait_key_async() is the coroutine
# built on it). That event belongs to the firmware, so it is adopted rather than
# owned: closing it here never closes ConIn's.

import uctypes

from _uefi import Event
from . import raw
from . import status
from .protocol import ProtocolDescriptor, Field, Method, Out, VPtr, U64, Bool, WStr, I32

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
        Method("query_mode", (U64, Out(U64), Out(U64))),  # (ModeNumber) -> (cols, rows)
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
        self._key_event = None  # lazily adopted in key_event()

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
        return self.out.query_mode(self.current_mode() if mode is None else mode)

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

    def key_event(self):
        """ConIn's WaitForKey as a pollable uefi.Event (created once, cached).

        The event belongs to the firmware, so this adopts it rather than owning
        it: closing the Console never closes ConIn's event. Being an Event, it
        drops into select / asyncio like any other, which is how you wait for a
        keystroke without blocking the scheduler.
        """
        ev = self._key_event
        if ev is None:
            ev = Event(ptr=self.inp.wait_for_key)
            self._key_event = ev
        return ev

    def wait_key(self):
        """Block until a key is pressed; return (scancode, char).

        Uses BS->WaitForEvent on the ConIn WaitForKey event — a hard block at
        TPL_APPLICATION (it does not yield to asyncio), which is what a modal boot
        menu wants. Use key_event() with select/asyncio to wait cooperatively.
        """
        ev = self.inp.wait_for_key
        while True:
            raw.wait_for_event([ev])
            k = self.read_key()
            if k is not None:
                return k

    async def wait_key_async(self):
        """Suspend the current asyncio task until a key is pressed; return it."""
        from .event import wait_async

        ev = self.key_event()
        while True:
            k = self.read_key()
            if k is not None:
                return k
            await wait_async(ev)

    def flush_input(self):
        """Discard any pending keystrokes."""
        while self.read_key() is not None:
            pass
