import sys
import time
import os
from machine import Pin
from machine import bitstream


# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    gpio_pin = "P12_1"
    input_pin = "P13_7"
elif "CY8CPROTO-063-BLE" in machine:
    gpio_pin = "P5_2"
    input_pin = "P12_7"

print("bitstream execution")
timing = [3000000, 1000000, 8000000, 5000000]
buf = bytearray([0xF0])
p0 = Pin(gpio_pin, Pin.OUT, value=0)
input_pin = Pin(input_pin, Pin.IN)
while input_pin.value() == 1:
    pass
for i in range(2):
    bitstream(p0, 0, timing, buf)


# Wait for listener

# Send bitstream
