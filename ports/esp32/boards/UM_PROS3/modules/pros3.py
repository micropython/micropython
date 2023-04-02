# ProS3 MicroPython Helper Library
# MIT license; Copyright (c) 2022 Seon Rozenblum - Unexpected Maker
#
# Project home:
# http://pros3.io

# Import required libraries
from micropython import const
from machine import Pin, ADC
import time

# ProS3 Hardware Pin Assignments

# Sense Pins
VBUS_SENSE = const(33)
VBAT_SENSE = const(10)

# RGB LED & LDO2 Pins
RGB_DATA = const(18)
LDO2 = const(17)

# SPI
SPI_MOSI = const(35)
SPI_MISO = const(37)
SPI_CLK = const(36)

# I2C
I2C_SDA = const(8)
I2C_SCL = const(9)


# Helper functions
def set_ldo2_power(state):
    """Enable or Disable power to the second LDO"""
    Pin(LDO2, Pin.OUT).value(state)


def get_battery_voltage():
    """
    Returns the current battery voltage. If no battery is connected, returns 4.2V which is the charge voltage
    This is an approximation only, but useful to detect if the charge state of the battery is getting low.
    """
    adc = ADC(Pin(VBAT_SENSE))  # Assign the ADC pin to read
    measuredvbat = adc.read()
    measuredvbat /= 4095  # divide by 4095 as we are using the default ADC attenuation of 0dB
    measuredvbat *= 4.2  # Multiply by 4.2V, our max charge voltage for a 1S LiPo
    return round(measuredvbat, 2)


def get_vbus_present():
    """Detect if VBUS (5V) power source is present"""
    return Pin(VBUS_SENSE, Pin.IN).value() == 1


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
