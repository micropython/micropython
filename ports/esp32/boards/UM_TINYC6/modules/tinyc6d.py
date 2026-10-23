# TinyC6[D] Helper Library
# MIT license; Copyright (c) 2026 Seon Rozenblum - Unexpected Maker
#
# Project home:
#   https://tinyc6.io

# Import required libraries
from machine import Pin, I2C
from max17048 import MAX17048
from fxl6408 import FXL6408
import machine

# IO expander pin assignments
ANTENNA_SELECT = 0  # High = external uFL, Low = onboard

# Initialize I2C bus
i2c = I2C(0, scl=Pin.board.I2C_SCL, sda=Pin.board.I2C_SDA)

# Create an instance of the MAX17048 class
max17048 = MAX17048(i2c)

# Create an instance of the FXL6408 IO expander
io_expander = FXL6408(i2c)


# Helper functions
def get_bat_voltage():
    """Read the battery voltage from the fuel gauge"""
    return max17048.cell_voltage


def get_state_of_charge():
    """Read the battery state of charge from the fuel gauge"""
    return max17048.state_of_charge


def get_vbus_present():
    """Detect if VBUS (5V) power source is present"""
    return Pin(Pin.board.VBUS_SENSE, Pin.IN).value() == 1


def set_pixel_power(state):
    """Enable or Disable power to the onboard NeoPixel to either show colour, or to reduce power for deep sleep."""
    Pin(Pin.board.RGB_PWR, Pin.OUT).value(state)


def set_antenna_external(state):
    """Set the RF switch to the external uFL connector (True) or the onboard antenna (False)."""
    io_expander.config(ANTENNA_SELECT, FXL6408.OUT, value=state)


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


# Go into deep sleep but shut down the NeoPixel first to save power
# Use this if you want lowest deep sleep current
def go_deepsleep(t):
    """Deep sleep helper that also powers down the on-board NeoPixel."""
    set_pixel_power(False)
    machine.deepsleep(t)
