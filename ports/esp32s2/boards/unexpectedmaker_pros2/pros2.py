# Pros2 Helper Library
# 2020 Seon Rozenblum, Unexpected Maker
#
# Project home:
#   https://pros2.io
#

# Import required libraries
import time
import board
from digitalio import DigitalInOut, Direction, Pull
from analogio import AnalogIn
 

# Helper functions

def enable_LDO2(state):
    """Set the power for the second on-board LDO to allow no current draw when not needed."""

    # Grab a reference to the LDO2 IO (21 in this case)
    ldo2 = DigitalInOut(board.LDO2)
    ldo2.direction = Direction.OUTPUT

    # Set the LDO2 power pin on / off
    ldo2.value = state

    # A small delay to let the IO change state
    time.sleep(0.035)


# Dotstar rainbow colour wheel
def dotstar_color_wheel(wheel_pos):
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


# Get a *rough* estimate of the current battery voltage
# If the battery is not present, the charge IC will still report it's trying to charge at X voltage
# so it will still show a voltage.
def get_battery_voltage():
    """
    Returns the current battery voltage. If no battery is connected, returns 3.7V
    This is an approximation only, but useful to detect of the charge state of the battery is getting low.
    """
    adc = AnalogIn(board.VBAT_SENSE)

    measured_vbat = adc.value  # Read the value
    measured_vbat /= 4095  # divide by 4095 as we are using the default ADC voltage range of 0-1V
    measured_vbat *= 3.7  # Multiply by 3.7V, our reference voltage
    return measured_vbat


# Disable LDO2 by default
enable_LDO2(False)
