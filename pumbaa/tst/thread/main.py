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

import _thread
from sync import Event
import harness

EVENT = Event()


def thread_main(mask):
    EVENT.write(mask)


def test_print():
    help(_thread)


def test_start():
    mask = 0x1
    _thread.start_new_thread(thread_main, (mask, ))
    EVENT.read(mask)


def main():
    testcases = [
        (test_print, "test_print"),
        (test_start, "test_start")
    ]
    harness.run(testcases)


if __name__ == '__main__':
    main()
