# Copyright © 2020 by Thorsten von Eicken.
from machine import Pin as mPin
from micropython import const

# MCP23017 registers - defined as const(), which makes them module-global
IODIR = const(0)
IOCON = const(0xA)
GPPU = const(0xC)
GPIO = const(0x12)
OLAT = const(0x14)


# MCP23017 is a minimal driver for an 16-bit I2C I/O expander
class MCP23017:
    def __init__(self, i2c, addr=0x20):
        self.i2c = i2c
        self.addr = addr
        self.write(IOCON, 0x00)
        self.write2(IODIR, 0xFF, 0xFF)  # all inputs
        self.gpio0 = 0
        self.gpio1 = 0
        self.write2(GPIO, 0, 0)

    # read an 8-bit register, internal method
    def read(self, reg):
        return self.i2c.readfrom_mem(self.addr, reg, 1)[0]

    # write an 8-bit register, internal method
    def write(self, reg, v):
        self.i2c.writeto_mem(self.addr, reg, bytes((v,)))

    # write two 8-bit registers, internal method
    def write2(self, reg, v1, v2):
        self.i2c.writeto_mem(self.addr, reg, bytes((v1, v2)))

    # writebuf writes multiple bytes to the same register
    def writebuf(self, reg, v):
        self.i2c.writeto_mem(self.addr, reg, v)

    # bit reads or sets a bit in a register, caching the gpio register for performance
    def bit(self, reg, num, v=None):
        if v is None:
            data = self.read(reg)
            if reg == GPIO:
                self.gpio0 = data
            elif reg == GPIO + 1:
                self.gpio1 = data
            return (data >> num) & 1
        else:
            mask = 0xFF ^ (1 << num)
            if reg == GPIO:
                self.gpio0 = (self.gpio0 & mask) | ((v & 1) << num)
                self.write(reg, self.gpio0)
            elif reg == GPIO + 1:
                self.gpio1 = (self.gpio1 & mask) | ((v & 1) << num)
                self.write(reg, self.gpio1)
            else:
                data = (self.read(reg) & mask) | ((v & 1) << num)
                self.write(reg, data)

    def pin(self, num, mode=mPin.IN, pull=None, value=None):
        return Pin(self, num, mode, pull, value)


# Pin implements a minimal machine.Pin look-alike for pins on the MCP23017
class Pin:
    def __init__(self, mcp23017, num, mode=mPin.IN, pull=None, value=None):
        self.bit = mcp23017.bit
        incr = num >> 3  # bank selector
        self.gpio = GPIO + incr
        self.num = num = num & 0x7
        if value is not None:
            self.bit(self.gpio, num, value)
        self.bit(IODIR + incr, num, 1 if mode == mPin.IN else 0)
        self.bit(GPPU + incr, num, 1 if pull == mPin.PULL_UP else 0)

    # value reads or write a pin value (0 or 1)
    def value(self, v=None):
        if v is None:
            return self.bit(self.gpio, self.num)
        else:
            self.bit(self.gpio, self.num, v)

    __call__ = value
