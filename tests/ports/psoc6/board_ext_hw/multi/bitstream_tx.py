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
board = os.uname().machine
if "CY8CPROTO-062-4343W" in board:
    bitstream_pin_name = "P12_1"
    wait_signal_pin_name = "P12_3"
elif "CY8CPROTO-063-BLE" in board:
    bitstream_pin_name = "P5_2"
    wait_signal_pin_name = "P6_2"
elif "CY8CKIT-062S2-AI" in board:
    bitstream_pin_name = "P9_5"
    wait_signal_pin_name = "P9_7"

signal_received = False


def signal_irq(arg):
    global signal_received
    signal_received = True


def wait_for_rx_ready():
    global signal_received
    wait_signal_pin = Pin(wait_signal_pin_name, Pin.IN)
    wait_signal_pin.irq(handler=signal_irq, trigger=Pin.IRQ_RISING)
    while not signal_received:
        pass

    signal_received = False
    wait_signal_pin.deinit()
    # print("rx ready")


def send_bitstream():
    timing = [3000000, 1000000, 8000000, 5000000]
    buf = bytearray([0xF0])
    bitstream_pin = Pin(bitstream_pin_name, Pin.OUT, value=0)
    for i in range(2):
        bitstream(bitstream_pin, 0, timing, buf)

    bitstream_pin.deinit()


# print("bitstream tx")
wait_for_rx_ready()
send_bitstream()
