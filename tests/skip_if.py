# SPDX-FileCopyrightText: Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
# SPDX-FileCopyrightText: 2014 MicroPython & CircuitPython contributors (https://github.com/adafruit/circuitpython/graphs/contributors)
#
# SPDX-License-Identifier: MIT

# This must be on one line so its skipped when built into tests.
"""This file provides helpers to detect particular running conditions and skip the test when appropriate."""


def skip():
    print("SKIP")
    raise SystemExit


def always():
    skip()


def no_reversed():
    import builtins

    if "reversed" not in dir(builtins):
        skip()


def no_bigint():
    try:
        # We have to use variables because 1 << 40 causes an exception on parse and
        # cannot be caught.
        x = 40
        x = 1 << x
    except OverflowError:
        skip()


def board_in(*board):
    try:
        import test_env
    except ImportError:

        class Env:
            def __init__(self, board):
                self.board = board

        test_env = Env("unknown")
    if test_env.board in board:
        skip()


def board_not_in(*board):
    try:
        import test_env
    except ImportError:

        class Env:
            def __init__(self, board):
                self.board = board

        test_env = Env("unknown")
    if test_env.board not in board:
        skip()


def no_cpython_compat():
    try:
        from collections import namedtuple
    except ImportError:
        skip()
    try:
        T3 = namedtuple("TupComma", "foo bar")
    except TypeError:
        skip()


def no_slice_assign():
    try:
        memoryview
    except:
        skip()
    b1 = bytearray(b"1234")
    b2 = bytearray(b"5678")
    m1 = memoryview(b1)
    m2 = memoryview(b2)
    try:
        m2[1:3] = m1[0:2]
    except TypeError:
        skip()


def no_reverse_ops():
    class Foo:
        def __radd__(self, other):
            pass

    try:
        5 + Foo()
    except TypeError:
        skip()
