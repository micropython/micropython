import os
from machine import Pin, Signal
import machine
import time

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    pin1_name = "P13_7"
    pin2_name = "P13_6"
elif "CY8CPROTO-063-BLE" in machine:
    pin1_name = "P6_3"
    pin2_name = "P6_2"

# Pin out and pin in must be connected
# together in the board

pin_out = Pin(pin1_name, mode=Pin.OUT, value=True)
pin_in = Pin(pin2_name, Pin.IN)

# Set signal for output pin as non-inverted
print("non-inverted")

signal = Signal(pin_out, invert=False)

signal.on()
print("signal is high when on(): ", pin_in.value() == 1)
print("signal is active when on(): ", signal.value() == True)

signal.value(0)
print("signal is high when value(0): ", pin_in.value() == 0)
print("signal is inactive when value(0): ", signal.value() == False)

signal.value(1)
print("signal is high when value(1): ", pin_in.value() == 1)
print("signal is active when value(1): ", signal.value() == True)

signal.off()
print("signal is low when off(): ", pin_in.value() == 0)
print("signal is inactive when off(): ", signal.value() == False)

print("\n")

# Set signal for output pin as inverted
print("inverted")
signal = Signal(pin_out, invert=True)

signal.on()
print("signal is low when on(): ", pin_in.value() == 0)
print("signal is active when on(): ", signal.value() == True)

signal.value(0)
print("signal is high when value(0): ", pin_in.value() == 1)
print("signal is inactive when value(0): ", signal.value() == False)

signal.value(1)
print("signal is low when value(1): ", pin_in.value() == 0)
print("signal is active when value(1): ", signal.value() == True)

signal.off()
print("signal is high when off(): ", pin_in.value() == 1)
print("signal is inactive when off(): ", signal.value() == False)
