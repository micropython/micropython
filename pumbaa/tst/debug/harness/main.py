#
# @file main.py
#
# @section License
# Copyright (C) 2016, Erik Moqvist
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# This file is part of the Pumbaa project.
#

import harness
from harness import TestCaseSkippedError, SuiteError
from harness import assert_raises, ExceptionNotRaisedError


def test_assert_raises():
    with assert_raises(RuntimeError, "expected error"):
        raise RuntimeError("expected error")

    try:
        with assert_raises(TypeError):
            raise ValueError("unexpected error")
    except ValueError as e:
        assert str(e) == "unexpected error"

    try:
        with assert_raises(TypeError):
            pass
    except ExceptionNotRaisedError as e:
        assert str(e) == "<class 'TypeError'> not raised"


def test_passed():
    pass


def test_skipped():
    raise TestCaseSkippedError()


def test_failed():
    raise Exception()


def main():
    testcases = [
        (test_assert_raises, "test_assert_raises"),
        (test_passed, "test_passed"),
        (test_skipped, "test_skipped"),
        (test_failed, "test_failed")
    ]

    try:
        harness.run(testcases)
        raise RuntimeError()
    except harness.SuiteError as e:
        if e.failed != 1 or e.skipped != 1:
            raise RuntimeError()


if __name__ == '__main__':
    main()
