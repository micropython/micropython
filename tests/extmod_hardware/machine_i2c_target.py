# Test machine.I2CTarget.
#
# IMPORTANT: This test requires hardware connections: a SoftI2C instance must be
# wired to a hardware I2C target.  See pin definitions below.

import sys

try:
    from machine import Pin, SoftI2C, I2CTarget
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest

ADDR = 67

kwargs_target = {}

# Configure pins based on the target.
if sys.platform == "alif" and sys.implementation._build == "ALIF_ENSEMBLE":
    args_controller = {"scl": "P1_1", "sda": "P1_0"}
    args_target = (0,)  # on pins P0_3/P0_2
elif sys.platform == "esp32":
    args_controller = {"scl": 5, "sda": 6}
    args_target = (0,)  # on pins 9/8 for C3 and S3, 18/19 for others
    kwargs_target = {"scl": 9, "sda": 8}
elif sys.platform == "rp2":
    args_controller = {"scl": 5, "sda": 4}
    args_target = (1,)
elif sys.platform == "pyboard":
    if sys.implementation._build == "NUCLEO_WB55":
        args_controller = {"scl": "B8", "sda": "B9"}
        args_target = (3,)
    else:
        args_controller = {"scl": "X1", "sda": "X2"}
        args_target = ("X",)
elif "zephyr-nucleo_wb55rg" in sys.implementation._machine:
    # PB8=I2C1_SCL, PB9=I2C1_SDA (on Arduino header D15/D14)
    # PC0=I2C3_SCL, PC1=I2C3_SDA (on Arduino header A0/A1)
    args_controller = {"scl": Pin(("gpiob", 8)), "sda": Pin(("gpiob", 9))}
    args_target = ("i2c3",)
elif "zephyr-rpi_pico" in sys.implementation._machine:
    args_controller = {"scl": Pin(("gpio0", 5)), "sda": Pin(("gpio0", 4))}
    args_target = ("i2c1",)  # on gpio7/gpio6
elif sys.platform == "mimxrt":
    if "Teensy" in sys.implementation._machine:
        args_controller = {"scl": "A6", "sda": "A3"}  # D20/D17
    else:
        args_controller = {"scl": "D0", "sda": "D1"}
    args_target = (0,)  # pins 19/18 On Teensy 4.x
elif sys.platform == "samd":
    args_controller = {"scl": "D5", "sda": "D1"}
    args_target = ()
else:
    print("Please add support for this test on this platform.")
    raise SystemExit


def config_pull_up():
    Pin(args_controller["scl"], Pin.OPEN_DRAIN, Pin.PULL_UP)
    Pin(args_controller["sda"], Pin.OPEN_DRAIN, Pin.PULL_UP)


class TestMemory(unittest.TestCase):
    @classmethod
    def setUpClass(cls):
        cls.mem = bytearray(8)
        cls.i2c = SoftI2C(**args_controller)
        cls.i2c_target = I2CTarget(*args_target, **kwargs_target, addr=ADDR, mem=cls.mem)
        config_pull_up()

    @classmethod
    def tearDownClass(cls):
        cls.i2c_target.deinit()

    def test_scan(self):
        self.assertIn(ADDR, self.i2c.scan())

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

    @unittest.skipIf(sys.platform == "esp32", "write lengths larger than buffer unsupported")
    def test_write_wrap_large(self):
        self.mem[:] = b"01234567"
        self.i2c.writeto_mem(ADDR, 0, b"testTESTmore")
        self.assertEqual(self.mem, bytearray(b"moreTEST"))

    def test_read(self):
        self.mem[:] = b"01234567"
        self.assertEqual(self.i2c.readfrom_mem(ADDR, 0, 4), b"0123")
        self.assertEqual(self.i2c.readfrom_mem(ADDR, 4, 4), b"4567")

    def test_read_wrap(self):
        self.mem[:] = b"01234567"
        self.assertEqual(self.i2c.readfrom_mem(ADDR, 0, 4), b"0123")
        self.assertEqual(self.i2c.readfrom_mem(ADDR, 2, 4), b"2345")
        self.assertEqual(self.i2c.readfrom_mem(ADDR, 6, 4), b"6701")

    @unittest.skipIf(sys.platform == "esp32", "read lengths larger than buffer unsupported")
    def test_read_wrap_large(self):
        self.mem[:] = b"01234567"
        self.assertEqual(self.i2c.readfrom_mem(ADDR, 0, 12), b"012345670123")

    def test_write_read(self):
        self.mem[:] = b"01234567"
        self.assertEqual(self.i2c.writeto(ADDR, b"\x02"), 1)
        self.assertEqual(self.i2c.readfrom(ADDR, 4), b"2345")

    @unittest.skipIf(sys.platform == "esp32", "read after read unsupported")
    def test_write_read_read(self):
        self.mem[:] = b"01234567"
        self.assertEqual(self.i2c.writeto(ADDR, b"\x02"), 1)
        self.assertEqual(self.i2c.readfrom(ADDR, 4), b"2345")
        self.assertEqual(self.i2c.readfrom(ADDR, 4), b"7012")


@unittest.skipUnless(hasattr(I2CTarget, "IRQ_END_READ"), "IRQ unsupported")
class TestMemoryIRQ(unittest.TestCase):
    @staticmethod
    def irq_handler(i2c_target):
        flags = i2c_target.irq().flags()
        TestMemoryIRQ.events[TestMemoryIRQ.num_events] = flags
        TestMemoryIRQ.events[TestMemoryIRQ.num_events + 1] = i2c_target.memaddr
        TestMemoryIRQ.num_events += 2

    @classmethod
    def setUpClass(cls):
        cls.mem = bytearray(8)
        cls.events = [0] * 8
        cls.num_events = 0
        cls.i2c = SoftI2C(**args_controller)
        cls.i2c_target = I2CTarget(*args_target, **kwargs_target, addr=ADDR, mem=cls.mem)
        cls.i2c_target.irq(TestMemoryIRQ.irq_handler)
        config_pull_up()

    @classmethod
    def tearDownClass(cls):
        cls.i2c_target.deinit()

    @unittest.skipIf(sys.platform == "esp32", "scan doesn't trigger IRQ_END_WRITE")
    def test_scan(self):
        TestMemoryIRQ.num_events = 0
        self.i2c.scan()
        self.assertEqual(self.events[: self.num_events], [I2CTarget.IRQ_END_WRITE, 0])

    def test_write(self):
        TestMemoryIRQ.num_events = 0
        self.mem[:] = b"01234567"
        self.i2c.writeto_mem(ADDR, 2, b"test")
        self.assertEqual(self.mem, bytearray(b"01test67"))
        self.assertEqual(self.events[: self.num_events], [I2CTarget.IRQ_END_WRITE, 2])

    def test_read(self):
        TestMemoryIRQ.num_events = 0
        self.mem[:] = b"01234567"
        self.assertEqual(self.i2c.readfrom_mem(ADDR, 2, 4), b"2345")
        self.assertEqual(self.events[: self.num_events], [I2CTarget.IRQ_END_READ, 2])


@unittest.skipUnless(hasattr(I2CTarget, "IRQ_WRITE_REQ"), "IRQ unsupported")
@unittest.skipIf(sys.platform == "mimxrt", "not working")
@unittest.skipIf(sys.platform == "pyboard", "can't queue more than one byte")
@unittest.skipIf(sys.platform == "samd", "not working")
@unittest.skipIf(sys.platform == "zephyr", "must call readinto/write in IRQ handler")
class TestPolling(unittest.TestCase):
    @staticmethod
    def irq_handler(i2c_target, buf=bytearray(1)):
        flags = i2c_target.irq().flags()
        if flags & I2CTarget.IRQ_READ_REQ:
            i2c_target.write(b"0123")

    @classmethod
    def setUpClass(cls):
        cls.i2c = SoftI2C(**args_controller)
        cls.i2c_target = I2CTarget(*args_target, addr=ADDR)
        cls.i2c_target.irq(
            TestPolling.irq_handler,
            I2CTarget.IRQ_WRITE_REQ | I2CTarget.IRQ_READ_REQ,
            hard=True,
        )
        config_pull_up()

    @classmethod
    def tearDownClass(cls):
        cls.i2c_target.deinit()

    def test_read(self):
        # Can't write data up front, must wait until IRQ_READ_REQ.
        # self.assertEqual(self.i2c_target.write(b"abcd"), 4)
        self.assertEqual(self.i2c.readfrom(ADDR, 4), b"0123")

    def test_write(self):
        # Can do the read outside the IRQ, but requires IRQ_WRITE_REQ trigger to be set.
        self.assertEqual(self.i2c.writeto(ADDR, b"0123"), 4)
        buf = bytearray(8)
        self.assertEqual(self.i2c_target.readinto(buf), 4)
        self.assertEqual(buf, b"0123\x00\x00\x00\x00")


@unittest.skipUnless(hasattr(I2CTarget, "IRQ_ADDR_MATCH_READ"), "IRQ unsupported")
class TestIRQ(unittest.TestCase):
    @staticmethod
    def irq_handler(i2c_target, buf=bytearray(1)):
        flags = i2c_target.irq().flags()
        TestIRQ.events[TestIRQ.num_events] = flags
        TestIRQ.num_events += 1
        if flags & I2CTarget.IRQ_READ_REQ:
            i2c_target.write(b"Y")
        if flags & I2CTarget.IRQ_WRITE_REQ:
            i2c_target.readinto(buf)
            TestIRQ.events[TestIRQ.num_events] = buf[0]
            TestIRQ.num_events += 1

    @classmethod
    def setUpClass(cls):
        cls.events = [0] * 8
        cls.num_events = 0
        cls.i2c = SoftI2C(**args_controller)
        cls.i2c_target = I2CTarget(*args_target, addr=ADDR)
        cls.i2c_target.irq(
            TestIRQ.irq_handler,
            I2CTarget.IRQ_ADDR_MATCH_READ
            | I2CTarget.IRQ_ADDR_MATCH_WRITE
            | I2CTarget.IRQ_WRITE_REQ
            | I2CTarget.IRQ_READ_REQ
            | I2CTarget.IRQ_END_READ
            | I2CTarget.IRQ_END_WRITE,
            hard=True,
        )
        config_pull_up()

    @classmethod
    def tearDownClass(cls):
        cls.i2c_target.deinit()

    def test_scan(self):
        TestIRQ.num_events = 0
        self.i2c.scan()
        self.assertEqual(
            self.events[: self.num_events],
            [
                I2CTarget.IRQ_ADDR_MATCH_WRITE,
                I2CTarget.IRQ_END_WRITE,
            ],
        )

    def test_write(self):
        TestIRQ.num_events = 0
        self.i2c.writeto(ADDR, b"XYZ")
        self.assertEqual(
            self.events[: self.num_events],
            [
                I2CTarget.IRQ_ADDR_MATCH_WRITE,
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"X"),
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"Y"),
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"Z"),
                I2CTarget.IRQ_END_WRITE,
            ],
        )

    def test_read(self):
        TestIRQ.num_events = 0
        self.assertEqual(self.i2c.readfrom(ADDR, 1), b"Y")
        self.assertEqual(
            self.events[: self.num_events],
            [
                I2CTarget.IRQ_ADDR_MATCH_READ,
                I2CTarget.IRQ_READ_REQ,
                I2CTarget.IRQ_READ_REQ,
                I2CTarget.IRQ_END_READ,
            ],
        )

    def test_write_read(self):
        TestIRQ.num_events = 0
        self.i2c.writeto(ADDR, b"X", False)
        self.assertEqual(self.i2c.readfrom(ADDR, 1), b"Y")
        self.assertEqual(
            self.events[: self.num_events],
            [
                I2CTarget.IRQ_ADDR_MATCH_WRITE,
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"X"),
                I2CTarget.IRQ_END_WRITE,
                I2CTarget.IRQ_ADDR_MATCH_READ,
                I2CTarget.IRQ_READ_REQ,
                I2CTarget.IRQ_READ_REQ,
                I2CTarget.IRQ_END_READ,
            ],
        )


if __name__ == "__main__":
    unittest.main()
