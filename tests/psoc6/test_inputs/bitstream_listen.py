import os
import time
from machine import Pin
from machine import bitstream

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    pin_in = "P12_0"

elif "CY8CPROTO-063-BLE" in machine:
    pin_in = "P5_3"

print("bitstream capture")
input_pin = Pin(pin_in, Pin.IN)
value = input_pin.value()
periods = []
last_value = 0
for i in range(100):
    start_time = time.ticks_us()
    while input_pin.value() == last_value:
        pass

    time_period = time.ticks_diff(time.ticks_us(), start_time)
    periods.append((time_period, last_value))
    last_value = input_pin.value()
        

    

