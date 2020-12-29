# NeoPixel driver for MicroPython on ESP32
# MIT license; Copyright (c) 2016 Damien P. George

from esp import neopixel_write, neopixel_write_timings
from machine import freq


def pixelbitstream(pin, timing_ns, buf):
    if len(timing_ns) == 4:
        t0high, t0low, t1high, t1low = timing_ns
        t_latch = 0  # Will not wait for latch
    elif len(timing_ns) == 5:
        t0high, t0low, t1high, t1low, t_latch = timing_ns
    else:
        raise Exception("pixelbitstream expected 4-tuple or 5-tuple of timing_ns")

    ticks_per_ns = freq() / 1_000_000_000.0
    t0high = int(t0high * ticks_per_ns)
    t0low = int(t0low * ticks_per_ns)
    t1high = int(t1high * ticks_per_ns)
    t1low = int(t1low * ticks_per_ns)
    t_latch = int(t_latch * ticks_per_ns)

    neopixel_write_timings(pin, buf, t0high, t0low, t1high, t1low, t_latch)


class NeoPixel:
    ORDER = (1, 0, 2, 3)
    WS2811_TIMING = 0
    WS2812S_TIMING = 1
    WS2812B_TIMING = 2
    
    def __init__(self, pin, n, bpp=3, timing=WS2812S_TIMING):
        self.pin = pin
        self.n = n
        self.bpp = bpp
        self.buf = bytearray(n * bpp)
        self.pin.init(pin.OUT)
        self.timing = timing

    def __setitem__(self, index, val):
        offset = index * self.bpp
        for i in range(self.bpp):
            self.buf[offset + self.ORDER[i]] = val[i]

    def __getitem__(self, index):
        offset = index * self.bpp
        return tuple(self.buf[offset + self.ORDER[i]]
                     for i in range(self.bpp))

    def fill(self, color):
        for i in range(self.n):
            self[i] = color

    def write(self):
        neopixel_write(self.pin, self.buf, self.timing)
