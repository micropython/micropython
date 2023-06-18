# Example using PIO to turn on an LED via an explicit exec.
# Note: this does not work on Pico W because it uses Pin(25) for LED output.
#
# Demonstrates:
#   - using set_init and set_base
#   - using StateMachine.exec

import time
from machine import Pin
import rp2


# Define an empty program that uses a single set pin.
@rp2.asm_pio(set_init=rp2.PIO.OUT_LOW)
def prog():
    pass


# Construct the StateMachine, binding Pin(25) to the set pin.
sm = rp2.StateMachine(0, prog, set_base=Pin(25))

# Turn on the set pin via an exec instruction.
sm.exec("set(pins, 1)")

# Sleep for 500ms.
time.sleep(0.5)

# Turn off the set pin via an exec instruction.
sm.exec("set(pins, 0)")
