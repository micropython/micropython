# The MIT License (MIT)
#
# Copyright (c) 2017 Scott Shawcroft for Adafruit Industries
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

# This must be on one line so its skipped when built into tests.
"""This file provides helpers to detect particular running conditions and skip the test when appropriate."""

def skip():
    print("SKIP")
    raise SystemExit

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
        try:
            from collections import namedtuple
        except ImportError:
            from ucollections import namedtuple
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
    b1 = bytearray(b'1234')
    b2 = bytearray(b'5678')
    m1 = memoryview(b1)
    m2 = memoryview(b2)
    try:
        m2[1:3] = m1[0:2]
    except TypeError:
        skip()
