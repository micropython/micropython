# APA102 driver for MicroPython on ESP8266
# MIT license; Copyright (c) 2016 Robert Foss, Daniel Busch

from esp import apa102_write
from neopixel import NeoPixel


class APA102(NeoPixel):
    ORDER = (0, 1, 2, 3)

    def __init__(self, clock_pin, data_pin, n, bpp=4):
        super().__init__(data_pin, n, bpp)
        self.clock_pin = clock_pin
        self.clock_pin.init(clock_pin.OUT)

    def write(self):
        apa102_write(self.clock_pin, self.pin, self.buf)
