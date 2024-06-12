import os
import time
from machine import Pin
from machine import bitstream

""""
This script is running together with the bitstream_tx.py test.
One pin generates a synchronisation signal to inform the transmitter that receiver is ready, and
another is configured as an input, which is used for receiving the bitstream signals.
"""

# Allocate pin based on board
board = os.uname().machine
if "CY8CPROTO-062-4343W" in board:
    bitstream_in_pin_name = "P12_1"
    rx_ready_signal_pin_name = "P13_5"
elif "CY8CPROTO-063-BLE" in board:
    bitstream_in_pin_name = "P5_2"
    rx_ready_signal_pin_name = "P6_2"
elif "CY8CKIT-062S2-AI" in board:
    bitstream_in_pin_name = "P9_6"
    rx_ready_signal_pin_name = "P9_7"

expected_values = [
    8000,
    5000,
    8000,
    5000,
    8000,
    5000,
    8000,
    5000,
    3000,
    1000,
    3000,
    1000,
    3000,
    1000,
    3000,
    1000,
]
tolerance = 100


def notify_readiness_to_tx():
    rx_ready_signal_pin = Pin(
        rx_ready_signal_pin_name, Pin.OUT, value=0
    )  # signal to inform the transmitter that receiver is read
    rx_ready_signal_pin.low()
    # delay
    for i in range(1000):
        pass
    rx_ready_signal_pin.high()
    rx_ready_signal_pin.deinit()


def bitstream_rx_measure():
    global periods
    periods = []
    last_value = 0
    bitstream_in_pin = Pin(bitstream_in_pin_name, Pin.IN)
    start_time = time.ticks_us()
    current_value = 0

    for i in range(17):
        while current_value == last_value:
            current_value = bitstream_in_pin.value()
        current_time = time.ticks_us()
        time_period = time.ticks_diff(current_time, start_time)
        last_value = current_value
        start_time = current_time
        periods.append(time_period)

    bitstream_in_pin.deinit()


def validate_bitstream():
    for i in range(len(periods) - 1):
        diff = abs(periods[i + 1] - expected_values[i])
        if diff <= tolerance:
            print("true")
        else:
            print("false")
            print(
                "expected :"
                + str(expected_values[i])
                + " period: "
                + str(periods[i + 1])
                + " diff: "
                + str(diff)
            )


print("bitstream rx")
notify_readiness_to_tx()
bitstream_rx_measure()
validate_bitstream()
