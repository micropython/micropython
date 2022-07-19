"""
The MIT License (MIT)

Copyright (c) 2013-2022 Ibrahim Abdelkader <iabdalkader@openmv.io>
Copyright (c) 2013-2022 Kwabena W. Agyeman <kwagyeman@openmv.io>

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

HTS221 driver driver for MicroPython.
Original source: https://github.com/ControlEverythingCommunity/HTS221/blob/master/Python/HTS221.py

Example usage:

import time
import hts221
from machine import Pin, I2C

bus = I2C(1, scl=Pin(15), sda=Pin(14))
hts = hts221.HTS221(bus)

while (True):
    rH   = hts.humidity()
    temp = hts.temperature()
    print ("rH: %.2f%% T: %.2fC" %(rH, temp))
    time.sleep_ms(100)
"""

import struct


class HTS221:
    def __init__(self, i2c, data_rate=1, address=0x5F):
        self.bus = i2c
        self.odr = data_rate
        self.slv_addr = address

        # Set configuration register
        # Humidity and temperature average configuration
        self.bus.writeto_mem(self.slv_addr, 0x10, b"\x1B")

        # Set control register
        # PD | BDU | ODR
        cfg = 0x80 | 0x04 | (self.odr & 0x3)
        self.bus.writeto_mem(self.slv_addr, 0x20, bytes([cfg]))

        # Read Calibration values from non-volatile memory of the device
        # Humidity Calibration values
        self.H0 = self._read_reg(0x30, 1) / 2
        self.H1 = self._read_reg(0x31, 1) / 2
        self.H2 = self._read_reg(0x36, 2)
        self.H3 = self._read_reg(0x3A, 2)

        # Temperature Calibration values
        raw = self._read_reg(0x35, 1)
        self.T0 = ((raw & 0x03) * 256) + self._read_reg(0x32, 1)
        self.T1 = ((raw & 0x0C) * 64) + self._read_reg(0x33, 1)
        self.T2 = self._read_reg(0x3C, 2)
        self.T3 = self._read_reg(0x3E, 2)

    def _read_reg(self, reg_addr, size):
        fmt = "B" if size == 1 else "H"
        reg_addr = reg_addr if size == 1 else reg_addr | 0x80
        return struct.unpack(fmt, self.bus.readfrom_mem(self.slv_addr, reg_addr, size))[0]

    def humidity(self):
        rH = self._read_reg(0x28, 2)
        return (self.H1 - self.H0) * (rH - self.H2) / (self.H3 - self.H2) + self.H0

    def temperature(self):
        temp = self._read_reg(0x2A, 2)
        if temp > 32767:
            temp -= 65536
        return ((self.T1 - self.T0) / 8.0) * (temp - self.T2) / (self.T3 - self.T2) + (
            self.T0 / 8.0
        )
