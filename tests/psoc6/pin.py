import os
from machine import Pin
import time
import machine

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    pin1_name = "P13_7"
    pin2_name = "P0_4"
elif "CY8CPROTO-063-BLE" in machine:
    pin1_name = "P6_3"
    pin2_name = "P0_4"

# Tests for Output pin
# Instantiate output pin for led operation
# Constr
p1 = Pin(pin1_name)
p1.init(Pin.OUT)

p1.value(0)  # Led glows
print(p1.value())  # None

p1.off()  # Switches off led
print(p1.off())  # None

p1.value(False)  # Led glows
print(p1.value())  # None

# Tests for Input pin
# Instantiate input pin for button operation
p2 = Pin(pin2_name, Pin.IN)
p2.value()
print(p2.value())  # 1 - button is released
