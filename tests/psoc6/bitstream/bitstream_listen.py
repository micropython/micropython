import os
import time
from machine import Pin
from machine import bitstream

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    pin_in = "P12_1"
    pin_out = "P13_7"

elif "CY8CPROTO-063-BLE" in machine:
    pin_in = "P5_3"
    pin_out = "P12_6"

expected_values = [
    8000,
    5000,
    8000,
    5000,
    8000,
    50000,
    8000,
    5000,
    3000,
    1000,
    3000,
    10000,
    3000,
    1000,
    3000,
    1000,
]
tolerance = 100

print("bitstream capture")
input_pin = Pin(pin_in, Pin.IN)
periods = []
last_value = 0
p0 = Pin(pin_out, Pin.OUT, value=0)
start_time = time.ticks_us()
current_value = 0
for i in range(17):
    while current_value == last_value:
        current_value = input_pin.value()
    current_time = time.ticks_us()
    time_period = time.ticks_diff(current_time, start_time)
    last_value = current_value
    start_time = current_time
    periods.append(time_period)

for i in range(len(periods) - 1):
    if abs((periods[i + 1] - expected_values[i]) <= tolerance):
        print("true")
    else:
        print("false")


# Notify readiness to sender

# Measure

# Validate
