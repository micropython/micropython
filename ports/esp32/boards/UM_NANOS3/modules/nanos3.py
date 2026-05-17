# NanoS3 Helper Library
# MIT license; Copyright (c) 2023 Seon Rozenblum - Unexpected Maker
#
# Project home:
#   https://nanos3.io

# Import required libraries
from micropython import const
from machine import Pin, ADC
import time

# TinyS3 Hardware Pin Assignments

# RGB LED Pins
RGB_DATA = const(41)
RGB_PWR = const(42)

# SPI
SPI_MOSI = const(35)
SPI_MISO = const(37)
SPI_CLK = const(36)

# I2C
I2C_SDA = const(8)
I2C_SCL = const(9)


# Helper functions
def set_pixel_power(state):
    """Enable or Disable power to the onboard NeoPixel to either show colour, or to reduce power for deep sleep."""
    Pin(RGB_PWR, Pin.OUT).value(state)


# NeoPixel rainbow colour wheel
def rgb_color_wheel(wheel_pos):
    """Color wheel to allow for cycling through the rainbow of RGB colors."""
    wheel_pos = wheel_pos % 255

    if wheel_pos < 85:
        return 255 - wheel_pos * 3, 0, wheel_pos * 3
    elif wheel_pos < 170:
        wheel_pos -= 85
        return 0, wheel_pos * 3, 255 - wheel_pos * 3
    else:
        wheel_pos -= 170
        return wheel_pos * 3, 255 - wheel_pos * 3, 0
