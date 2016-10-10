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
from drivers import Dac
import board
import harness
from harness import assert_raises


def test_print():
    print(Dac)
    dac = Dac(board.PIN_DAC0)
    print(dac)


def test_output():
    # Single pin.
    dac = Dac(board.PIN_DAC0)
    dac.convert("\x03\xff")

    # List of pins.
    dac = Dac([board.PIN_DAC0, board.PIN_DAC1], 11025)
    dac.convert(bytearray(16 * "\x03\xff"))
    dac.convert(16 * "\x03\xff")
    dac.async_convert(16 * "\x03\xff")
    dac.async_wait()


def test_bad_arguments():
    # Too many devices.
    with assert_raises(ValueError, "too many devices"):
        Dac([0, 1, 2])
        
    # Bad devices type.
    with assert_raises(TypeError, "bad devices"):
        Dac(None)

    # Bad pin.
    with assert_raises(ValueError, "bad pin"):
        Dac(board.PIN_LED)


def main():
    testcases = [
        (test_print, "test_print"),
        (test_output, "test_output"),
        (test_bad_arguments, "test_bad_arguments")
    ]
    harness.run(testcases)


if __name__ == '__main__':
    main()
