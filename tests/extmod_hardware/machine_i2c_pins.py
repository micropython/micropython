# Test retrieving the SCL/SDA pins of an I2C bus.
#
# IMPORTANT: This test does not need any external wiring, but it does
# reconfigure the pins listed below, so they must be free to use.  The pins
# mirror those used by extmod_hardware/machine_i2c_target.py.

import sys

try:
    from machine import Pin, SoftI2C
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest

try:
    from machine import I2C
except ImportError:
    I2C = None

# Arguments for the hardware I2C constructor, or None if this platform has no
# hardware I2C that can be constructed without further wiring information.
args_hard = None
kwargs_hard = {}

# Configure pins based on the target.
if sys.platform == "alif" and sys.implementation._build == "ALIF_ENSEMBLE":
    args_soft = {"scl": "P1_1", "sda": "P1_0"}
    args_hard = (0,)  # on pins P0_3/P0_2
elif sys.platform == "esp32":
    args_soft = {"scl": 5, "sda": 6}
    args_hard = (0,)
    kwargs_hard = {"scl": 9, "sda": 8}
elif sys.platform == "rp2":
    args_soft = {"scl": 5, "sda": 4}
    args_hard = (1,)  # on gpio7/gpio6
# Note: the stm32 port reports sys.platform as "pyboard" on all of its boards.
elif sys.platform == "pyboard":
    if sys.implementation._build == "NUCLEO_WB55":
        args_soft = {"scl": "B8", "sda": "B9"}
        args_hard = (3,)
    elif sys.implementation._build == "ARDUINO_NICLA_VISION":
        # SCL/SDA are the I2C1 pads; the hardware bus tested is I2C3 (PA8/PC9).
        args_soft = {"scl": "SCL", "sda": "SDA"}
        args_hard = (3,)
    elif sys.implementation._build.startswith("PYB"):
        args_soft = {"scl": "X1", "sda": "X2"}
        args_hard = ("X",)
    else:
        print("Please add support for this test on this platform.")
        raise SystemExit
elif "zephyr-nucleo_wb55rg" in sys.implementation._machine:
    # PB8=I2C1_SCL, PB9=I2C1_SDA (on Arduino header D15/D14)
    args_soft = {"scl": Pin(("gpiob", 8)), "sda": Pin(("gpiob", 9))}
    args_hard = ("i2c3",)
elif "zephyr-rpi_pico" in sys.implementation._machine:
    args_soft = {"scl": Pin(("gpio0", 5)), "sda": Pin(("gpio0", 4))}
    args_hard = ("i2c1",)  # on gpio7/gpio6
elif sys.platform == "renesas-ra":
    if sys.implementation._build == "ARDUINO_PORTENTA_C33":
        # I2C0 is on P408/P407, and this port does not accept explicit pins
        # for the hardware bus, so the soft bus uses the same pins.
        args_soft = {"scl": "P408", "sda": "P407"}
        args_hard = (0,)
    else:
        print("Please add support for this test on this platform.")
        raise SystemExit
elif sys.platform == "mimxrt":
    if "Teensy" in sys.implementation._machine:
        args_soft = {"scl": "A6", "sda": "A3"}  # D20/D17
    else:
        args_soft = {"scl": "D0", "sda": "D1"}
    args_hard = (0,)  # pins 19/18 on Teensy 4.x
elif sys.platform == "samd":
    args_soft = {"scl": "D5", "sda": "D1"}
    args_hard = ()  # board default I2C on its default pins
else:
    print("Please add support for this test on this platform.")
    raise SystemExit


# Pin objects are not guaranteed to be singletons on every port, so compare
# pins by their representation rather than by identity.
def assert_is_pin(test, pin, expected=None):
    test.assertIsInstance(pin, Pin)
    if expected is not None:
        test.assertEqual(repr(pin), repr(Pin(expected)))


class TestSoftI2C(unittest.TestCase):
    def setUp(self):
        self.i2c = SoftI2C(**args_soft)

    def test_pins(self):
        assert_is_pin(self, self.i2c.scl(), args_soft["scl"])
        assert_is_pin(self, self.i2c.sda(), args_soft["sda"])

    def test_pins_after_init(self):
        # Swapping the pins over must be reflected by the accessors.
        self.i2c.init(scl=args_soft["sda"], sda=args_soft["scl"])
        assert_is_pin(self, self.i2c.scl(), args_soft["sda"])
        assert_is_pin(self, self.i2c.sda(), args_soft["scl"])


class TestHardI2C(unittest.TestCase):
    # Note: the bus is created inside the test rather than in setUp() because
    # setUp() also runs for skipped tests.
    @unittest.skipIf(I2C is None or args_hard is None, "no hardware I2C")
    def test_pins(self):
        i2c = I2C(*args_hard, **kwargs_hard)
        try:
            scl = i2c.scl()
            sda = i2c.sda()
            if scl is None:
                # This port does not report the pins of a hardware I2C bus.
                self.assertIsNone(sda)
                return
            assert_is_pin(self, scl, kwargs_hard.get("scl", None))
            assert_is_pin(self, sda, kwargs_hard.get("sda", None))
        finally:
            # I2C.deinit() is not available on all firmware versions.
            if hasattr(i2c, "deinit"):
                i2c.deinit()


if __name__ == "__main__":
    unittest.main()
