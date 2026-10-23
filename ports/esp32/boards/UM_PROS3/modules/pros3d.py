# ProS3[D] Helper Library
# MIT license; Copyright (c) 2026 Seon Rozenblum - Unexpected Maker
#
# Project home:
#   https://pros3.io

# Import required libraries
from machine import Pin, I2C
from max17048 import MAX17048

# Initialize I2C bus
i2c = I2C(0, scl=Pin.board.I2C_SCL, sda=Pin.board.I2C_SDA)

# Create an instance of the MAX17048 class
max17048 = MAX17048(i2c)


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


def set_ldo2_power(state):
    """Enable or Disable power to the second LDO"""
    Pin(Pin.board.LDO2_PWR, Pin.OUT).value(state)


def set_antenna_external(state):
    """Set the RF switch to the external uFL connector (True) or the onboard antenna (False)."""
    Pin(Pin.board.ANT_SWITCH, Pin.OUT).value(state)


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
