# FeatherS2 Helper Library
# 2020 Seon Rozenblum, Unexpected Maker
#
# Project home:
#   https://feathers2.io
#

# Import required libraries
import time
import board
from digitalio import DigitalInOut, Direction, Pull
 

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



# Disable LDO2 by default
enable_LDO2(False)
