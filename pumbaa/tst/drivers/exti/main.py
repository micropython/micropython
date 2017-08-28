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

import os
from sync import Event
from drivers import Exti, Pin
import board
import harness
from harness import assert_raises


def test_print():
    print(Exti)
    event = Event()
    exti = Exti(board.EXTI_D0, Exti.RISING, event, 1)
    print(exti)


def test_falling_edge():
    pin = Pin(board.PIN_D4, Pin.OUTPUT)
    pin.write(1)

    event = Event()

    exti = Exti(board.EXTI_D3, Exti.FALLING, event, 0x1)
    exti.start()

    # Make sure no interrupt has already occured.
    assert event.size() == 0

    # Trigger the interrupt and wait for the event.
    pin.write(0)

    if not 'Linux' in os.uname().machine:
        print("Waiting for the interrupt to occur... ", end="")
        assert event.read(0x1) == 0x1
        print("ok.")

    exti.stop()


def test_bad_arguments():
    event = Event()

    # Bad device.
    with assert_raises(ValueError, "bad exti device -1"):
        Exti(-1, Exti.BOTH, event, 1)

    # Bad trigger.
    with assert_raises(ValueError, "bad trigger -1"):
        Exti(board.EXTI_D3, -1, event, 1)

    # Bad event channel.
    with assert_raises(TypeError, "expected <class 'Event'>"):
        Exti(board.EXTI_D3, Exti.BOTH, None, 1)

    # Bad event mask.
    with assert_raises(TypeError, "can't convert NoneType to int"):
        Exti(board.EXTI_D3, Exti.BOTH, event, None, 1)

    # Bad callback.
    with assert_raises(TypeError, "bad callback"):
        Exti(board.EXTI_D3, Exti.BOTH, event, 1, 1)


def main():
    testcases = [
        (test_print, "test_print"),
        (test_falling_edge, "test_falling_edge"),
        (test_bad_arguments, "test_bad_arguments")
    ]
    harness.run(testcases)


if __name__ == '__main__':
    main()
