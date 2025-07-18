# Test machine.I2CTarget.
#
# IMPORTANT: This test requires hardware connections: a SoftI2C instance must be
# wired to a hardware I2C target.

import sys

try:
    from machine import Pin, SoftI2C, I2CTarget
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest

ADDR = 67

# Configure pins based on the target.
if sys.platform == "alif" and sys.implementation._build == "ALIF_ENSEMBLE":
    args_controller = {"scl": "P1_1", "sda": "P1_0"}
    args_target = (0,)
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
elif sys.platform == "zephyr":
    if "zephyr-nucleo_wb55rg" in sys.implementation._machine:
        # PB8=I2C1_SCL, PB9=I2C1_SDA (on Arduino header D15/D14)
        # PC0=I2C3_SCL, PC1=I2C3_SDA (on Arduino header A0/A1)
        args_controller = {"scl": Pin(("gpiob", 8)), "sda": Pin(("gpiob", 9))}
        args_target = ("i2c3",)
    elif "zephyr-rpi_pico" in sys.implementation._machine:
        args_controller = {"scl": Pin(("gpio0", 5)), "sda": Pin(("gpio0", 4))}
        args_target = ("i2c1",)
    else:
        print("Please add support for this test on this zephyr platform.")
        raise SystemExit
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
        cls.i2c_target = I2CTarget(*args_target, addr=ADDR, mem=cls.mem)
        config_pull_up()

    @classmethod
    def tearDownClass(cls):
        cls.i2c_target.deinit()

    def test_scan(self):
        self.assertEqual(self.i2c.scan(), [ADDR])

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
            I2CTarget.IRQ_ADDR_MATCH
            | I2CTarget.IRQ_WRITE_REQ
            | I2CTarget.IRQ_READ_REQ
            | I2CTarget.IRQ_END,
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
                I2CTarget.IRQ_ADDR_MATCH,
                I2CTarget.IRQ_END,
            ],
        )

    def test_write(self):
        TestIRQ.num_events = 0
        self.i2c.writeto(ADDR, b"XYZ")
        self.assertEqual(
            self.events[: self.num_events],
            [
                I2CTarget.IRQ_ADDR_MATCH,
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"X"),
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"Y"),
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"Z"),
                I2CTarget.IRQ_END,
            ],
        )

    def test_read(self):
        TestIRQ.num_events = 0
        self.assertEqual(self.i2c.readfrom(ADDR, 1), b"Y")
        self.assertEqual(
            self.events[: self.num_events],
            [
                I2CTarget.IRQ_ADDR_MATCH,
                I2CTarget.IRQ_READ_REQ,
                I2CTarget.IRQ_READ_REQ,
                I2CTarget.IRQ_END,
            ],
        )

    def test_write_read(self):
        TestIRQ.num_events = 0
        self.i2c.writeto(ADDR, b"X", False)
        self.assertEqual(self.i2c.readfrom(ADDR, 1), b"Y")
        self.assertEqual(
            self.events[: self.num_events],
            [
                I2CTarget.IRQ_ADDR_MATCH,
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"X"),
                I2CTarget.IRQ_ADDR_MATCH,
                I2CTarget.IRQ_READ_REQ,
                I2CTarget.IRQ_READ_REQ,
                I2CTarget.IRQ_END,
            ],
        )


if __name__ == "__main__":
    unittest.main()
