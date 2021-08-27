# NeoPixel driver for MicroPython
# MIT license; Copyright (c) 2016 Damien P. George, 2021 Jim Mussared

from micropython import const
from machine import bitstream

_BITSTREAM_TYPE_HIGH_LOW = const(0)
_TIMING_WS2818_800 = (400, 850, 800, 450)
_TIMING_WS2818_400 = (800, 1700, 1600, 900)


class NeoPixel:
    ORDER = (1, 0, 2, 3)

    def __init__(self, pin, n, bpp=3, timing=1):
        self.pin = pin
        self.n = n
        self.bpp = bpp
        self.buf = bytearray(n * bpp)
        self.pin.init(pin.OUT)
        self.timing = (
            (_TIMING_WS2818_800 if timing else _TIMING_WS2818_400)
            if isinstance(timing, int)
            else timing
        )

    def __len__(self):
        return self.n

    def __setitem__(self, index, val):
        offset = index * self.bpp
        for i in range(self.bpp):
            self.buf[offset + self.ORDER[i]] = val[i]

    def __getitem__(self, index):
        offset = index * self.bpp
        return tuple(self.buf[offset + self.ORDER[i]] for i in range(self.bpp))

    def fill(self, color):
        for i in range(self.n):
            self[i] = color

    def write(self):
        bitstream(self.pin, _BITSTREAM_TYPE_HIGH_LOW, self.timing, self.buf)
