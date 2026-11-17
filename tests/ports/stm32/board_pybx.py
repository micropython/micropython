# Test board-specific items on PYBv1.x, PYBLITEv1.0 and PYBD-SFx boards.

import sys

build = sys.implementation._build

if not build.startswith("PYB"):
    print("SKIP")
    raise SystemExit

import machine
import pyb
import unittest

if build.startswith("PYBD_"):
    # PYBD-SFx boards.
    expected_uarts = [1, 2, 3, 4, 6, "XA", "YA", "YB"]
    expected_spis = [1, 2, 3, "X", "Y"]
    expected_i2cs = [1, 2, "X", "Y"]
    expected_cans = [1, "X"]
    if build == "PYBD_SF6":
        expected_uarts += [7]
        expected_cans += [2, "Y"]
else:
    # PYBv1.x and PYBLITEv1.0 boards.
    expected_uarts = [1, 2, 6, "XA", "XB", "YA"]
    expected_spis = [1, 2, "X", "Y"]
    expected_i2cs = [1, "X", "Y"]
    expected_cans = []
    if build.startswith("PYBV1"):
        expected_uarts += [3, 4, "YB"]
        expected_i2cs += [2]
        expected_cans += [1, 2, "YA", "YB"]
    elif build.startswith("PYBLITEV1"):
        expected_i2cs += [3]


class TestPeripheralConstruction(unittest.TestCase):
    def _test_peripheral(self, periph, args, expected):
        for bus in (0, 1, 2, 3, 4, 5, 6, 7, 8, "X", "Y", "Z", "XA", "XB", "YA", "YB"):
            try:
                periph(bus, *args)
                assert bus in expected
            except ValueError:
                assert bus not in expected

    def test_uart(self):
        self._test_peripheral(machine.UART, (9600,), expected_uarts)

    def test_spi(self):
        self._test_peripheral(machine.SPI, (), expected_spis)

    def test_i2c(self):
        self._test_peripheral(machine.I2C, (), expected_i2cs)

    @unittest.skipUnless(expected_cans, "no CAN")
    def test_can(self):
        self._test_peripheral(pyb.CAN, (pyb.CAN.LOOPBACK,), expected_cans)


if __name__ == "__main__":
    unittest.main()
