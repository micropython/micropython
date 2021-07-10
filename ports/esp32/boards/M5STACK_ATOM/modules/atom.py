# M5Stack ATOM MicroPython Helper Library
# MIT license; Copyright (c) 2021 IAMLIUBO work for M5STACK
#
# Hardware details:
#       ATOM Lite    https://docs.m5stack.com/en/core/atom_lite
#       ATOM Matrix  https://docs.m5stack.com/en/core/atom_matrix

from micropython import const
from machine import Pin
import neopixel

# M5STACK ATOM Hardware Pin Assignments
"""
          FRONT
                  |3V3|
|G21|   IR   G12  |G22|
|G25|   BTN  G39  |G19|
| 5V| WS2812  G27 |G23|
|GNG| MPU G21 G25 |G33|
     G32 G26 5V GND
       Grove Port
"""

# WS2812
WS2812_PIN = const(27)

# Button
BUTTON_PIN = const(39)

# IR
IR_PIN = const(12)

# I2C
I2C0_SCL_PIN = const(21)
I2C0_SDA_PIN = const(25)

# Grove port
GROVE_PORT_PIN = (const(26), const(32))


class ATOM:
    def __init__(self, np_n):
        self._np = neopixel.NeoPixel(pin=Pin(WS2812_PIN), n=np_n)
        self._btn = Pin(BUTTON_PIN, Pin.IN, Pin.PULL_UP)

    def get_button_status(self):
        return self._btn.value()

    def set_button_callback(self, cb):
        self._btn.irq(trigger=Pin.IRQ_FALLING, handler=cb)

    def set_pixel_color(self, num, r, g, b):
        if num <= self._np.n:
            self._np[num] = [r, g, b]
            self._np.write()

    def get_pixel_color(self, num):
        if num <= self._np.n:
            return self._np[num]

    def set_pixels_color(self, r, g, b):
        self._np.fill([r, g, b])
        self._np.write()


class Lite(ATOM):
    # WS2812 number: 1
    def __init__(self):
        super(Lite, self).__init__(np_n=1)


class Matrix(ATOM):
    # WS2812 number: 25
    def __init__(self):
        super(Matrix, self).__init__(np_n=25)
