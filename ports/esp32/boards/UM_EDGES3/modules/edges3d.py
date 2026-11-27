# EdgeS3[D] Helper Library
# MIT license; Copyright (c) 2026 Seon Rozenblum - Unexpected Maker
#
# Project home:
#   https://esp32s3.com/edges3d.html

# Import required libraries
from machine import Pin, I2C
from max17048 import MAX17048
from fxl6408 import FXL6408

# Initialize I2C bus
i2c = I2C(0, scl=Pin.board.I2C_SCL, sda=Pin.board.I2C_SDA)

# Create an instance of the MAX17048 class
max17048 = MAX17048(i2c)

# Create an instance of the FXL6408 IO expander (XIO0 - XIO7)
io_expander = FXL6408(i2c)


# Helper functions
def get_bat_voltage():
    """Read the battery voltage from the fuel gauge"""
    return max17048.cell_voltage


def get_state_of_charge():
    """Read the battery state of charge from the fuel gauge"""
    return max17048.state_of_charge


def set_antenna_external(state):
    """Set the RF switch to the external uFL connector (True) or the onboard antenna (False)."""
    Pin(Pin.board.ANT_SWITCH, Pin.OUT).value(state)
