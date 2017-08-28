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

from sync import Event
import harness
from harness import assert_raises


def test_help():
    event = Event()
    help(Event)
    help(event)


def test_read_write():
    event = Event()

    event.write(0x6)
    assert event.size() == 1
    assert event.read(0x2) == 0x2
    assert event.read(0x4) == 0x4


def test_bad_arguments():
    event = Event()

    with assert_raises(TypeError, "can't convert NoneType to int"):
        event.read(None)

    with assert_raises(TypeError, "can't convert NoneType to int"):
        event.write(None)


def main():
    testcases = [
        (test_help, "test_help"),
        (test_read_write, "test_read_write"),
        (test_bad_arguments, "test_bad_arguments")
    ]
    harness.run(testcases)


if __name__ == '__main__':
    main()
