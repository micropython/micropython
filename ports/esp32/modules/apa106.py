# APA106driver for MicroPython on ESP32
# MIT license; Copyright (c) 2016 Damien P. George

from neopixel import NeoPixel


class APA106(NeoPixel):
    ORDER = (0, 1, 2, 3)
