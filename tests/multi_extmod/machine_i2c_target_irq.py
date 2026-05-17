# Test I2CTarget IRQs and clock stretching.
#
# Requires two instances with their SCL and SDA lines connected together.
# Any combination of the below supported boards can be used.
#
# Notes:
# - pull-up resistors may be needed
# - alif use 1.8V signalling

import sys
import time
from machine import I2C, I2CTarget

if not hasattr(I2CTarget, "IRQ_ADDR_MATCH_READ"):
    print("SKIP")
    raise SystemExit

ADDR = 67
clock_stretch_us = 200

# Configure pins based on the target.
if sys.platform == "alif":
    i2c_args = (1,)  # pins P3_7/P3_6
    i2c_kwargs = {}
elif sys.platform == "mimxrt":
    i2c_args = (0,)  # pins 19/18 on Teensy 4.x
    i2c_kwargs = {}
    clock_stretch_us = 50  # mimxrt cannot delay too long in the IRQ handler
elif sys.platform == "rp2":
    i2c_args = (0,)
    i2c_kwargs = {"scl": 9, "sda": 8}
elif sys.platform == "pyboard":
    i2c_args = ("Y",)
    i2c_kwargs = {}
elif sys.platform == "samd":
    i2c_args = ()  # pins SCL/SDA
    i2c_kwargs = {}
elif "zephyr-rpi_pico" in sys.implementation._machine:
    i2c_args = ("i2c1",)  # on gpio7/gpio6
    i2c_kwargs = {}
else:
    print("Please add support for this test on this platform.")
    raise SystemExit


def simple_irq(i2c_target):
    flags = i2c_target.irq().flags()
    if flags & I2CTarget.IRQ_ADDR_MATCH_READ:
        print("IRQ_ADDR_MATCH_READ")
    if flags & I2CTarget.IRQ_ADDR_MATCH_WRITE:
        print("IRQ_ADDR_MATCH_WRITE")

    # Force clock stretching.
    time.sleep_us(clock_stretch_us)


class I2CTargetMemory:
    def __init__(self, i2c_target, mem):
        self.buf1 = bytearray(1)
        self.mem = mem
        self.memaddr = 0
        self.state = 0
        i2c_target.irq(
            self.irq,
            I2CTarget.IRQ_ADDR_MATCH_WRITE | I2CTarget.IRQ_READ_REQ | I2CTarget.IRQ_WRITE_REQ,
            hard=True,
        )

    def irq(self, i2c_target):
        # Force clock stretching.
        time.sleep_us(clock_stretch_us)

        flags = i2c_target.irq().flags()
        if flags & I2CTarget.IRQ_ADDR_MATCH_WRITE:
            self.state = 0
        if flags & I2CTarget.IRQ_READ_REQ:
            self.buf1[0] = self.mem[self.memaddr]
            self.memaddr += 1
            i2c_target.write(self.buf1)
        if flags & I2CTarget.IRQ_WRITE_REQ:
            i2c_target.readinto(self.buf1)
            if self.state == 0:
                self.state = 1
                self.memaddr = self.buf1[0]
            else:
                self.mem[self.memaddr] = self.buf1[0]
                self.memaddr += 1
        self.memaddr %= len(self.mem)

        # Force clock stretching.
        time.sleep_us(clock_stretch_us)


# I2C controller
def instance0():
    i2c = I2C(*i2c_args, **i2c_kwargs)
    multitest.next()
    for iteration in range(2):
        print("controller iteration", iteration)
        multitest.wait("target stage 1")
        i2c.writeto_mem(ADDR, 2, "0123")
        multitest.broadcast("controller stage 2")
        multitest.wait("target stage 3")
        print(i2c.readfrom_mem(ADDR, 2, 4))
        multitest.broadcast("controller stage 4")
    print("done")


# I2C target
def instance1():
    multitest.next()

    for iteration in range(2):
        print("target iteration", iteration)
        buf = bytearray(b"--------")
        if iteration == 0:
            # Use built-in memory capability of I2CTarget.
            i2c_target = I2CTarget(*i2c_args, **i2c_kwargs, addr=ADDR, mem=buf)
            i2c_target.irq(
                simple_irq,
                I2CTarget.IRQ_ADDR_MATCH_READ | I2CTarget.IRQ_ADDR_MATCH_WRITE,
                hard=True,
            )
        else:
            # Implement a memory device by hand.
            i2c_target = I2CTarget(*i2c_args, **i2c_kwargs, addr=ADDR)
            I2CTargetMemory(i2c_target, buf)

        multitest.broadcast("target stage 1")
        multitest.wait("controller stage 2")
        print(buf)
        multitest.broadcast("target stage 3")
        multitest.wait("controller stage 4")

        i2c_target.deinit()

    print("done")
