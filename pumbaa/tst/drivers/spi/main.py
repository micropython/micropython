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
from drivers import Spi
import board
import harness
from harness import assert_raises


def test_print():
    print(Spi)
    spi = Spi(board.SPI_0, board.PIN_D3)
    print(spi)


def test_data_transfer():
    spi = Spi(board.SPI_0, board.PIN_D3)
    spi.start()
    spi.take_bus()
    spi.select()
    read = spi.transfer(b'foo')
    assert len(read) == 3
    read = spi.transfer(b'foo', 1)
    assert len(read) == 1
    buf = bytearray(3)
    assert spi.transfer_into(buf, b'bar') == 3
    print(buf)
    assert spi.transfer_into(buf, b'bar', 2) == 2
    print(buf)
    read = spi.read(3)
    print(read)
    assert len(read) == 3
    buf = bytearray(3)
    assert spi.read_into(buf) == 3
    assert spi.read_into(buf, 1) == 1
    assert spi.write(b'fie') == 3
    assert spi.write(b'fie', 2) == 2
    spi.deselect()
    spi.give_bus()
    spi.stop()


def test_bad_arguments():
    # Bad device.
    with assert_raises(ValueError, "bad device"):
        Spi(100, 0)

    # Bad slave select.
    with assert_raises(ValueError, "bad slave select"):
        Spi(board.SPI_0, -1)

    # Bad mode.
    with assert_raises(ValueError, "bad mode"):
        Spi(board.SPI_0, board.PIN_D3, -1)

    # Bad polarity.
    with assert_raises(ValueError, "bad polarity"):
        Spi(board.SPI_0, board.PIN_D3, Spi.MODE_MASTER, 1, -1)

    # Bad phase.
    with assert_raises(ValueError, "bad phase"):
        Spi(board.SPI_0, board.PIN_D3, Spi.MODE_MASTER, 1, 1, -1)


def main():
    testcases = [
        (test_print, "test_print"),
        (test_data_transfer, "test_data_transfer"),
        (test_bad_arguments, "test_bad_arguments")
    ]
    harness.run(testcases)


if __name__ == '__main__':
    main()
