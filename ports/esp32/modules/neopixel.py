# NeoPixel driver for MicroPython on ESP32
# MIT license; Copyright (c) 2016 Damien P. George

from esp import neopixel_write


class NeoPixel:
    ORDER = (1, 0, 2, 3)
    
    def __init__(self, pin, n, brightness=0.5, bpp=3, timing=1):
        self.pin = pin
        self.n = n
        self.bpp = bpp
        self.buf = bytearray(n * bpp)
        self.pin.init(pin.OUT)
        self.timing = timing
        self.brightness = brightness

    def __setitem__(self, index, val):
        offset = index * self.bpp
        for i in range(self.bpp):
            self.buf[offset + self.ORDER[i]] = int(round(val[i]*self.brightness))

    def __getitem__(self, index):
        offset = index * self.bpp
        return tuple(self.buf[offset + self.ORDER[i]]
                     for i in range(self.bpp))

    def fill(self, color):
        for i in range(self.n):
            self[i] = color

    def write(self):
        neopixel_write(self.pin, self.buf, self.timing)
