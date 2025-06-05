# Test machine.I2CTargetMemory.
#
# IMPORTANT: This test requires hardware connections: two I2C instances must be
# be wired together.

import sys

try:
    from machine import Pin, SoftI2C, I2CTargetMemory
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest

ADDR = 67

# Configure pins based on the target.
if sys.platform == "rp2":
    args_controller = {"scl": 5, "sda": 4}
    args_target = (1,)

    def config_pull_up():
        Pin(5, Pin.OPEN_DRAIN, Pin.PULL_UP)
        Pin(4, Pin.OPEN_DRAIN, Pin.PULL_UP)
elif sys.platform == "pyboard":
    args_controller = {"scl": "X1", "sda": "X2"}
    args_target = ("X",)

    def config_pull_up():
        Pin("X1", Pin.OPEN_DRAIN, Pin.PULL_UP)
        Pin("X2", Pin.OPEN_DRAIN, Pin.PULL_UP)
else:
    print("Please add support for this test on this platform.")
    raise SystemExit


class Test(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.mem = bytearray(8)
        cls.i2c = SoftI2C(**args_controller)
        cls.i2c_mem = I2CTargetMemory(*args_target, addr=ADDR, mem=cls.mem)
        config_pull_up()

    @classmethod
    def tearDownClass(cls):
        cls.i2c_mem.deinit()

    def test_scan(self):
        self.assertEqual(self.i2c.scan(), [ADDR])

    def test_read(self):
        self.mem[:] = b"01234567"
        self.assertEqual(self.i2c.readfrom_mem(ADDR, 0, 4), b"0123")
        self.assertEqual(self.i2c.readfrom_mem(ADDR, 4, 4), b"4567")

    def test_read_wrap(self):
        self.mem[:] = b"01234567"
        self.assertEqual(self.i2c.readfrom_mem(ADDR, 0, 4), b"0123")
        self.assertEqual(self.i2c.readfrom_mem(ADDR, 2, 4), b"2345")
        self.assertEqual(self.i2c.readfrom_mem(ADDR, 6, 4), b"6701")
        self.assertEqual(self.i2c.readfrom_mem(ADDR, 0, 12), b"012345670123")

    def test_write(self):
        self.mem[:] = b"01234567"
        self.i2c.writeto_mem(ADDR, 0, b"test")
        self.assertEqual(self.mem, bytearray(b"test4567"))
        self.i2c.writeto_mem(ADDR, 4, b"TEST")
        self.assertEqual(self.mem, bytearray(b"testTEST"))

    def test_write_wrap(self):
        self.mem[:] = b"01234567"
        self.i2c.writeto_mem(ADDR, 6, b"test")
        self.assertEqual(self.mem, bytearray(b"st2345te"))
        self.i2c.writeto_mem(ADDR, 0, b"testTESTmore")
        self.assertEqual(self.mem, bytearray(b"moreTEST"))


if __name__ == "__main__":
    unittest.main()
