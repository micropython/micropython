import sys
import time
import os
from machine import Pin
from machine import bitstream

"""
This script is running together with the bitstream_rx.py test.
One pin generates the test bitstream pattern, and another is configured as 
an input, which is used for synchronization with the receiver.
"""

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    bitstream_pin_name = "P12_1"
    wait_signal_pin_name = "P13_7"
elif "CY8CPROTO-063-BLE" in machine:
    bitstream_pin_name = "P5_2"
    wait_signal_pin_name = "P12_7"


def wait_for_rx_ready():
    wait_signal_pin = Pin(wait_signal_pin_name, Pin.IN)
    while wait_signal_pin.value() == 1:
        pass


def send_bitstream():
    timing = [3000000, 1000000, 8000000, 5000000]
    buf = bytearray([0xF0])
    bitstream_pin = Pin(bitstream_pin_name, Pin.OUT, value=0)
    for i in range(2):
        bitstream(bitstream_pin, 0, timing, buf)


print("bitstream tx")
wait_for_rx_ready()
send_bitstream()
