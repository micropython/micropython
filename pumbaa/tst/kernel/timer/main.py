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
from kernel import Timer
from sync import Event
from harness import assert_raises


def test_help():
    event = Event()
    timer = Timer(1, event, 0x1)
    help(Timer)
    help(timer)


def test_single_shot_timer():
    event = Event()
    timer = Timer((0, 1000000), event, 0x1)
    print("starting single shot timer")
    timer.start()
    event.read(0x1)
    print("timeout")


def test_periodic_timer():
    event = Event()
    timer = Timer(0.2, event, 0x1, flags=Timer.PERIODIC)
    print("starting periodic timer")
    timer.start()
    for i in range(3):
        event.read(0x1)
        print("timeout", i)
    timer.stop()


def test_empty():
    Timer(1.0)


def test_bad_arguments():
    # Too long tuple.
    with assert_raises(TypeError, "expected tuple of length 2"):
        Timer((1, 0, 0), None, 1)

    # Bad timeout arguement.
    with assert_raises(TypeError, "can't convert NoneType to float"):
        Timer(None, None, 1)

    # Wrong type of second argument.
    with assert_raises(TypeError, "expected <class 'Event'>"):
        Timer(1, 1, 1)


def main():
    testcases = [
        (test_help, "test_help"),
        (test_single_shot_timer, "test_single_shot_timer"),
        (test_periodic_timer, "test_periodic_timer"),
        (test_empty, "test_empty"),
        (test_bad_arguments, "test_bad_arguments")
    ]
    harness.run(testcases)


if __name__ == '__main__':
    main()
