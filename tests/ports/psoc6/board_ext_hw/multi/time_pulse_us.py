from machine import Pin, time_pulse_us
import os
import time

# Allocate pin based on board
board = os.uname().machine
if "CY8CPROTO-062-4343W" in board:
    pulse_in_pin = "P12_1"
    ack_out_pin = "P12_3"
elif "CY8CPROTO-063-BLE" in board:
    pulse_in_pin = "P5_2"
    ack_out_pin = "P6_2"
elif "CY8CKIT-062S2-AI" in board:
    pulse_in_pin = "P9_2"
    ack_out_pin = "P9_7"


def blocking_delay_ms(delay_ms):
    start = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), start) < delay_ms:
        pass


pulse_in = Pin(pulse_in_pin, Pin.IN, Pin.PULL_DOWN)
ack_out = Pin(ack_out_pin, Pin.OUT, value=0)
ack_out.low()

width = 0

blocking_delay_ms(2000)

# Send begin ack to start generating pulse
ack_out.high()

# Measure the pulse width
width = time_pulse_us(pulse_in, 1, 10000000)

print(
    f"Pulse timing verified: {True if (0.98 < (width / 1000000) < 1.2) else 'False, width=' + str(width / 1000000)}"
)

# Deinitialize the pins
pulse_in.deinit()
ack_out.deinit()
