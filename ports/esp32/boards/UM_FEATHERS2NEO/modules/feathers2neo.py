# FeatherS2 Neo MicroPython Helper Library
# 2021 Seon Rozenblum - Unexpected Maker
#
# Project home:
#   https://unexpectedmaker.com/feathers2-neo
#
# 2021-Sep-04 - v0.1 - Initial implementation

# Import required libraries
from micropython import const
from machine import Pin, ADC
import machine, time

# FeatherS2 Neo Hardware Pin Assignments

# Sense Pins
VBUS_SENSE = const(34)
VBAT_SENSE = const(2)

# RGB LED Pins
RGB_DATA = const(40)
RGB_PWR = const(39)

# RGB MATRIX LED Pins
RGB_MATRIX_DATA = const(21)
RGB_MATRIX_PWR = const(4)

# SPI
SPI_MOSI = const(35)
SPI_MISO = const(37)
SPI_CLK = const(36)

# I2C
I2C_SDA = const(8)
I2C_SCL = const(9)

# DAC
DAC1 = const(17)
DAC2 = const(18)


# Helper functions
def set_pixel_power(state):
    """Enable or Disable power to the onboard NeoPixel to either show colour, or to reduce power for deep sleep."""
    Pin(RGB_PWR, Pin.OUT).value(state)


def set_pixel_matrix_power(state):
    """Enable or Disable power to the onboard NeoPixel RGB Matrix to either show colours, or to reduce power for deep sleep."""
    Pin(RGB_MATRIX_PWR, Pin.OUT).value(state)


def get_battery_voltage():
    """
    Returns the current battery voltage. If no battery is connected, returns 4.2V which is the charge voltage
    This is an approximation only, but useful to detect if the charge state of the battery is getting low.
    """
    adc = ADC(Pin(VBAT_SENSE))  # Assign the ADC pin to read
    measuredvbat = adc.read()  # Read the value
    measuredvbat /= 8192  # divide by 8192 as we are using the default ADC voltage range of 0-1V
    measuredvbat *= 4.2  # Multiply by 4.2V, our reference voltage
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


# Go into deep sleep but shut down the RGB LED first to save power
# Use this if you want lowest deep sleep current
def go_deepsleep(t):
    """Deep sleep helper that also powers down the on-board NeoPixel."""
    set_pixel_power(False)
    set_pixel_matrix_power(False)
    machine.deepsleep(t)
