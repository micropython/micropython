# NeoPixel driver for MicroPython on ESP32
# MIT license; Copyright (c) 2016 Damien P. George

try:
    from esp import neopixel_write
except:
    from pyb import neopixel_write


class NeoPixel:
    ORDER = (1, 0, 2, 3)
    WS400 = (500, 2000, 1200, 1300) # 400kHz, UNTESTED!
    WS2812S = (350, 800, 700, 600)
    
    def __init__(self, pin, n, bpp=3, timing=1):
        self.pin = pin
        self.n = n
        self.bpp = bpp
        self.buf = bytearray(n * bpp)
        self.pin.init(pin.OUT)
        if timing == 0:
            self.timing = self.WS400
        elif timing == 1:
            self.timing = self.WS2812S
        else:
            self.timing = timing # user supplied 4-tuple

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
        neopixel_write(self.pin, self.timing, self.buf)
