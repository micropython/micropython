# Test machine.I2CTarget.
#
# IMPORTANT: This test requires hardware connections: a SoftI2C instance must be
# wired to a hardware I2C target.
#
# requires: slice target-wiring

import sys
import time

try:
    from machine import Pin, I2CTarget
except ImportError:
    print("SKIP")
    raise SystemExit

import unittest

print("SKIP")
raise SystemExit

fail_because_this_test_crashes_pybd_sf2

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


class SoftI2C:
    def __init__(self, scl, sda):
        self.scl = Pin(scl, Pin.OPEN_DRAIN, Pin.PULL_UP)
        self.sda = Pin(sda, Pin.OPEN_DRAIN, Pin.PULL_UP)
        self.stop()

    def delay(self):
        return
        time.sleep_us(1)

    def scl_release(self):
        self.scl(1)
        self.delay()
        # Wait for SCL to be released, with timeout.
        while not self.scl():
            print("stretch")
            time.sleep_ms(5)

    def start(self):
        self.sda(1)
        self.delay()
        self.scl_release()
        self.sda(0)
        self.delay()

    def stop(self):
        self.delay()
        self.sda(0)
        self.delay()
        self.scl_release()
        self.sda(1)
        self.delay()

    # return value:
    #    0 - byte written and ack received
    #    1 - byte written and nack received
    def write_byte(self, val):
        self.delay()
        self.scl(0)

        for i in range(7, -1, -1):
            self.sda((val >> i) & 1)
            self.delay()
            self.scl_release()
            self.scl(0)

        self.sda(1)
        self.delay()
        self.scl_release()

        ack = self.sda()
        self.delay()
        self.scl(0)

        return ack

    # return value: byte received
    def read_byte(self, nack):
        self.delay()
        self.scl(0)
        self.delay()

        data = 0
        for i in range(7, -1, -1):
            self.scl_release()
            data = (data << 1) | self.sda()
            self.scl(0)
            self.delay()

        if not nack:
            self.sda(0)
        self.delay()
        self.scl_release()
        self.scl(0)
        self.sda(1)

        return data

    def transfer(self, addr, is_read, buf, stop):
        self.start()
        ret = self.write_byte(addr << 1 | is_read)
        if ret != 0:
            # nack received
            self.stop()
            return -1  # ENODEV

        if is_read:
            for i in range(len(buf)):
                buf[i] = self.read_byte(i + 1 == len(buf))
        else:
            for i in range(len(buf)):
                print("WR", i)
                ret = self.write_byte(buf[i])
                if ret != 0:
                    # nack received
                    print("NACK")
                    break

        if stop:
            self.stop()

    def writeto(self, addr, buf, stop=True):
        self.transfer(addr, False, buf, stop)

    def readfrom(self, addr, n):
        buf = bytearray(n)
        self.transfer(addr, True, buf, True)
        return buf

    def scan(self):
        result = []
        for addr in range(0x08, 0x78):
            ret = self.writeto(addr, b"")
            if ret == 0:
                result.append(addr)
        return result


import micropython

"""
def SoftTimer:
    def __init__(self, callback):
        self.callback = callback

    def sched(self, expiry_us):
        if time.ticks_diff(expiry_us, time.ticks_us()) >= 0:
            micropython.schedul(self.sched, expiry_us)
        else:
            self.callback(
    def start(self, t_us):
        expiry_us = time.ticks_add(time.ticks_us(), t_us))
        micropython.schedule(self.sched, expiry_us)
"""


from collections import deque

event_queue_t = deque((), 64)
event_queue_f = deque((), 64)
queued = False


def event_queue_add(delay_us, f):
    global queued
    # start_scheduler = len(event_queue_t) == 0
    expiry_us = time.ticks_add(time.ticks_us(), delay_us)
    event_queue_f.append(f)
    event_queue_t.append(expiry_us)
    if not queued:
        queued = True
        micropython.schedule(event_queue_process, None)


def event_queue_process(_):
    global queued
    queued = False
    expiry_us = event_queue_t[0]
    dt = time.ticks_diff(expiry_us, time.ticks_us())
    # print("process", dt)
    # print("do_work", expiry_us - time.ticks_us())
    if dt < 0:
        event_queue_t.popleft()
        flags = event_queue_f.popleft()
        event_queue_handle(flags)

    if len(event_queue_t) and not queued:
        # print('resched')
        micropython.schedule(event_queue_process, None)
        queued = True


def event_queue_handle(flags):
    print("handle", flags)
    buf = bytearray(1)

    i2c_target = gl_i2c_target

    TestIRQ.events[TestIRQ.num_events] = flags
    TestIRQ.num_events += 1
    if flags & I2CTarget.IRQ_READ_REQ:
        print("write Y")
        i2c_target.write(b"Y")
    if flags & I2CTarget.IRQ_WRITE_REQ:
        i2c_target.readinto(buf)
        TestIRQ.events[TestIRQ.num_events] = buf[0]
        TestIRQ.num_events += 1


class TestIRQ(unittest.TestCase):
    @staticmethod
    def irq_handler(i2c_target, buf=bytearray(1)):
        global gl_i2c_target
        gl_i2c_target = i2c_target

        flags = i2c_target.irq().flags()
        print("irq_handler", flags)
        event_queue_add(150_000, flags)
        return

        if flags & I2CTarget.IRQ_READ_REQ:
            expiry_us = time.ticks_add(time.ticks_us(), 50000)
            micropython.schedule(do_work, expiry_us)
        else:
            TestIRQ.events[TestIRQ.num_events] = flags
            TestIRQ.num_events += 1
            # if flags & I2CTarget.IRQ_READ_REQ:
            # i2c_target.write(b"Y")
            if flags & I2CTarget.IRQ_WRITE_REQ:
                i2c_target.readinto(buf)
                TestIRQ.events[TestIRQ.num_events] = buf[0]
                TestIRQ.num_events += 1

    @classmethod
    def setUpClass(cls):
        cls.events = [0] * 64
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
        while len(event_queue_t):
            pass
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
        self.assertEqual(self.i2c.readfrom(ADDR, 2), b"YY")
        while len(event_queue_t):
            pass
        self.assertEqual(
            self.events[: self.num_events],
            [
                I2CTarget.IRQ_ADDR_MATCH,
                I2CTarget.IRQ_READ_REQ,
                I2CTarget.IRQ_READ_REQ,
                I2CTarget.IRQ_READ_REQ,
                I2CTarget.IRQ_END,
            ],
        )

    def test_write_read(self):
        TestIRQ.num_events = 0
        self.i2c.writeto(ADDR, b"X", False)
        self.assertEqual(self.i2c.readfrom(ADDR, 1), b"Y")
        while len(event_queue_t):
            pass
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

    def test_write_large(self):
        TestIRQ.num_events = 0
        self.i2c.writeto(ADDR, b"01234567")
        while len(event_queue_t):
            pass
        self.assertEqual(
            self.events[: self.num_events],
            [
                I2CTarget.IRQ_ADDR_MATCH,
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"0"),
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"1"),
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"2"),
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"3"),
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"4"),
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"5"),
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"6"),
                I2CTarget.IRQ_WRITE_REQ,
                ord(b"7"),
                I2CTarget.IRQ_END,
            ],
        )


if __name__ == "__main__":
    unittest.main()
