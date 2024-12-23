# OMGS3 Helper Library
# MIT license; Copyright (c) 2024 Seon Rozenblum - Unexpected Maker
#
# Project home:
#   https://omgs3.io

# Import required libraries
from micropython import const
from machine import Pin, I2C
from max17048 import MAX17048
import time

# Initialize I2C bus
fg_i2c = I2C(0, scl=Pin.board.I2C_SCL, sda=Pin.board.I2C_SDA)

# Create an instance of the MAX17048 class
max17048 = MAX17048(fg_i2c)


# Helper functions
def get_bat_voltage():
    """Read the battery voltage from the fuel gauge"""
    voltage = max17048.cell_voltage
    print(f"Bat Voltage: {voltage}V")
    return voltage


def get_state_of_charge():
    """Read the battery state of charge from the fuel gauge"""
    soc = max17048.state_of_charge
    print(f"State of Charge: {soc}%")
    return soc


def get_vbus_present():
    """Detect if VBUS (5V) power source is present"""
    return Pin(Pin.board.VBUS_SENSE, Pin.IN).value() == 1


def set_pixel_power(state):
    """Enable or Disable power to the onboard NeoPixel to either show colour, or to reduce power for deep sleep."""
    Pin(Pin.board.RGB_PWR, Pin.OUT).value(state)


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
