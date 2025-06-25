# SPDX-FileCopyrightText: 2023 Jeff Epler for Adafruit Industries
#
# SPDX-License-Identifier: MIT

import os

from . import dang as curses

always = ["code.py", "boot.py", "settings.toml", "boot_out.txt"]
good_extensions = [".py", ".toml", ".txt", ".json"]


def os_exists(filename):
    try:
        os.stat(filename)
        return True
    except OSError:
        return False


def isdir(filename):
    st_mode = os.stat(filename)[0]
    r = st_mode & 0o40_000
    print(f"isdir {filename} {st_mode=:o} {r=}")
    return r


def has_good_extension(filename):
    for g in good_extensions:
        if filename.endswith(g):
            return True
    return False


def picker(stdscr, options, notes=(), start_idx=0):
    stdscr.erase()
    stdscr.addstr(curses.LINES - 1, 0, "Enter: select | ^C: quit")
    del options[curses.LINES - 1 :]
    for row, option in enumerate(options):
        if row < len(notes) and (note := notes[row]):
            option = f"{option} {note}"

        stdscr.addstr(row, 3, option)

    old_idx = None
    idx = start_idx
    while True:
        if idx != old_idx:
            if old_idx is not None:
                stdscr.addstr(old_idx, 0, "  ")
            stdscr.addstr(idx, 0, "=>")
            old_idx = idx

        k = stdscr.getkey()

        if k == "KEY_DOWN":
            idx = min(idx + 1, len(options) - 1)
        elif k == "KEY_UP":
            idx = max(idx - 1, 0)
        elif k in ("\r", "\n"):
            return options[idx]
        elif k == "\3":
            raise SystemExit


def pick_file():
    options = always[:] + sorted(
        (g for g in os.listdir("") if g not in always and not isdir(g) and not g.startswith(".")),
        key=lambda filename: (not has_good_extension(filename), filename),
    )
    notes = [None if os_exists(filename) else "(NEW)" for filename in options]
    return curses.wrapper(picker, options, notes)
