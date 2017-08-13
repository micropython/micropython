# APA102 driver for MicroPython on ESP8266
# MIT license; Copyright (c) 2016 Robert Foss, Daniel Busch

from esp import apa102_write

class APA102:
    def __init__(self, clock_pin, data_pin, n):
        self.clock_pin = clock_pin
        self.data_pin = data_pin
        self.n = n
        self.buf = bytearray(n * 4)

        self.clock_pin.init(clock_pin.OUT)
        self.data_pin.init(data_pin.OUT)

    def __setitem__(self, index, val):
        r, g, b, brightness = val
        self.buf[index * 4] = r
        self.buf[index * 4 + 1] = g
        self.buf[index * 4 + 2] = b
        self.buf[index * 4 + 3] = brightness

    def __getitem__(self, index):
        i = index * 4
        return self.buf[i], self.buf[i + 1], self.buf[i + 2], self.buf[i + 3]

    def write(self):
        apa102_write(self.clock_pin, self.data_pin, self.buf)
