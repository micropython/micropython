<!--
SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)

SPDX-License-Identifier: MIT
-->

# Building CircuitPython

Welcome to CircuitPython!

This document is a quick-start guide only.

Detailed guides on how to build CircuitPython can be found in the Adafruit Learn system at
https://learn.adafruit.com/building-circuitpython/

## Setup

Please ensure you setup your build environment appropriately, as per the guide.  You will need:

* Linux: https://learn.adafruit.com/building-circuitpython/linux
* MacOS: https://learn.adafruit.com/building-circuitpython/macos
* Windows Subsystem for Linux (WSL): https://learn.adafruit.com/building-circuitpython/windows-subsystem-for-linux

### Submodules

This project has a bunch of git submodules.  You will need to update them regularly.

    git submodule sync
    git submodule update --init

### mpy-cross

As part of the build process, mpy-cross is needed to compile .py files into .mpy files.
To compile (or recompile) mpy-cross:

    make -C mpy-cross

# Building

There a number of ports of CircuitPython!  To build for your board, change to the appropriate ports directory and build.

Examples:

    cd ports/atmel-samd
    make BOARD=circuitplayground_express

    cd ports/nrf
    make BOARD=circuitplayground_bluefruit

If you aren't sure what boards exist, have a peek in the boards subdirectory of your port.
If you have a fast computer with many cores, consider adding `-j` to your build flags, such as `-j17` on
a 6-core 12-thread machine.

# Testing

If you are working on changes to the core language, you might find it useful to run the test suite.
The test suite in the top level `tests` directory.  It needs the unix port to run.

    cd ports/unix
    make axtls
    make micropython

Then you can run the test suite:

    cd ../../tests
    ./run-tests

A successful run will say something like

    676 tests performed (19129 individual testcases)
    676 tests passed
    30 tests skipped: buffered_writer builtin_help builtin_range_binop class_delattr_setattr cmd_parsetree extra_coverage framebuf1 framebuf16 framebuf2 framebuf4 framebuf8 framebuf_subclass mpy_invalid namedtuple_asdict non_compliant resource_stream schedule sys_getsizeof urandom_extra ure_groups ure_span ure_sub ure_sub_unmatched vfs_basic vfs_fat_fileio1 vfs_fat_fileio2 vfs_fat_more vfs_fat_oldproto vfs_fat_ramdisk vfs_userfs

# Debugging

The easiest way to debug CircuitPython on hardware is with a JLink device, JLinkGDBServer, and an appropriate GDB.
Instructions can be found at https://learn.adafruit.com/debugging-the-samd21-with-gdb

If using JLink, you'll need both the `JLinkGDBServer` and `arm-none-eabi-gdb` running.

Example:

    JLinkGDBServer -if SWD -device ATSAMD51J19
    arm-none-eabi-gdb build-metro_m4_express/firmware.elf -iex "target extended-remote :2331"

If your port/build includes `arm-none-eabi-gdb-py`, consider using it instead, as it can be used for better register
debugging with https://github.com/bnahill/PyCortexMDebug

# Code Quality Checks

We apply code quality checks using pre-commit.  Install pre-commit once per system with

    python3 -mpip install pre-commit

Activate it once per git clone with

    pre-commit --install

Pre-commit also requires some additional programs to be installed through your package manager:

 * Standard Unix tools such as make, find, etc
 * The gettext package, any modern version
 * uncrustify version 0.71 (0.72 is also tested)

Each time you create a git commit, the pre-commit quality checks will be run.  You can also run them e.g., with `pre-commit run foo.c` or `pre-commit run --all` to run on all files whether modified or not.

Some pre-commit quality checks require your active attention to resolve, others (such as the formatting checks of uncrustify) are made automatically and must simply be incorporated into your code changes by committing them.
