import os
from machine import Pin
import time

# Allocate pin based on board
board = os.uname().machine
if "CY8CPROTO-062-4343W" in board:
    clk_in_pin = "P10_1"
    data_out_pin = "P10_5"
    sync_in_pin = "P10_0"
elif "CY8CPROTO-063-BLE" in board:
    print("SKIP")
    raise SystemExit
elif "CY8CKIT-062S2-AI" in board:
    print("SKIP")
    raise SystemExit

sig_val = 0
test_done = False


def signal_irq(event):
    global sig_val
    sig_val += 1


data_out = Pin(data_out_pin, mode=Pin.OUT, pull=Pin.PULL_DOWN, value=False)
clk_in = Pin(clk_in_pin, Pin.IN, Pin.PULL_DOWN)
sync_data = Pin(sync_in_pin, Pin.IN, Pin.PULL_DOWN)
sync_data.irq(handler=signal_irq, trigger=Pin.IRQ_RISING)

data_out.value(1)
while sig_val == 0:
    pass

data_out.value(0)
while sig_val == 1:
    pass

data_out.deinit()
clk_in.deinit()
sync_data.deinit()
