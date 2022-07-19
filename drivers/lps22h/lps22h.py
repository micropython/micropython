"""
The MIT License (MIT)

Copyright (c) 2016-2019 shaoziyang <shaoziyang@micropython.org.cn>
Copyright (c) 2022 Ibrahim Abdelkader <iabdalkader@openmv.io>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

LPS22HB/HH pressure sensor driver for MicroPython.

Example usage:

import time
from lps22h import LPS22H
from machine import Pin, I2C

bus = I2C(1, scl=Pin(15), sda=Pin(14))
lps = LPS22H(bus, oneshot=False)

while (True):
    print("Pressure: %.2f hPa Temperature: %.2f C"%(lps.pressure(), lps.temperature()))
    time.sleep_ms(10)
"""
import machine

_LPS22_CTRL_REG1 = const(0x10)
_LPS22_CTRL_REG2 = const(0x11)
_LPS22_STATUS = const(0x27)
_LPS22_TEMP_OUT_L = const(0x2B)
_LPS22_PRESS_OUT_XL = const(0x28)
_LPS22_PRESS_OUT_L = const(0x29)


class LPS22H:
    def __init__(self, i2c, address=0x5C, oneshot=False):
        self.i2c = i2c
        self.addr = address
        self.oneshot = oneshot
        self.buf = bytearray(1)
        # ODR=1 EN_LPFP=1 BDU=1
        self._write_reg(_LPS22_CTRL_REG1, 0x1A)
        self.set_oneshot_mode(self.oneshot)

    def _int16(self, d):
        return d if d < 0x8000 else d - 0x10000

    def _write_reg(self, reg, dat):
        self.buf[0] = dat
        self.i2c.writeto_mem(self.addr, reg, self.buf)

    def _read_reg(self, reg, width=8):
        self.i2c.readfrom_mem_into(self.addr, reg, self.buf)
        val = self.buf[0]
        if width == 16:
            val |= self._read_reg(reg + 1) << 8
        return val

    def _tigger_oneshot(self, b):
        if self.oneshot:
            self._write_reg(_LPS22_CTRL_REG2, self._read_reg(_LPS22_CTRL_REG2) | 0x01)
            self._read_reg(0x28 + b * 2)
            while True:
                if self._read_reg(_LPS22_STATUS) & b:
                    return
                machine.idle()

    def set_oneshot_mode(self, oneshot):
        self._read_reg(_LPS22_CTRL_REG1)
        self.oneshot = oneshot
        if oneshot:
            self.buf[0] &= 0x0F
        else:
            self.buf[0] |= 0x10
        self._write_reg(_LPS22_CTRL_REG1, self.buf[0])

    def pressure(self):
        if self.oneshot:
            self._tigger_oneshot(1)
        return (
            self._read_reg(_LPS22_PRESS_OUT_XL) + self._read_reg(_LPS22_PRESS_OUT_L, 16) * 256
        ) / 4096

    def temperature(self):
        if self.oneshot:
            self._tigger_oneshot(2)
        return self._int16(self._read_reg(_LPS22_TEMP_OUT_L, 16)) / 100

    def altitude(self):
        return (
            (((1013.25 / self.pressure()) ** (1 / 5.257)) - 1.0)
            * (self.temperature() + 273.15)
            / 0.0065
        )
