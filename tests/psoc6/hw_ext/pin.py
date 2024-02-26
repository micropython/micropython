import os
from machine import Pin
import machine
import time

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    pin1_name = "P13_7"
    pin2_name = "P13_6"
elif "CY8CPROTO-063-BLE" in machine:
    pin1_name = "P12_6"
    pin2_name = "P12_7"

# Pin out and pin in must be connected
# together in the board

pin_out = Pin(pin1_name, mode=Pin.OUT, value=True)
pin_in = Pin(pin2_name, Pin.IN)

# Validating initialization values
print("pin out initial value 1: ", pin_in.value() == 1)

pin_out.deinit()
pin_out = Pin(pin1_name, mode=Pin.OUT, value=False)
print("pin out initial value 0: ", pin_in.value() == 0)

# Validation different output setting
pin_out.value(1)
print("pin out value 1: ", pin_in.value() == 1)

pin_out.value(0)
print("pin out value 0: ", pin_in.value() == 0)

pin_out.value(True)
print("pin out value True: ", pin_in.value() == True)

pin_out.value(False)
print("pin out value False: ", pin_in.value() == False)

pin_out.high()
print("pin out value high: ", pin_in.value() == 1)

pin_out.low()
print("pin out value low: ", pin_in.value() == 0)

pin_out.on()
print("pin out value on: ", pin_in.value() == 1)

pin_out.off()
print("pin out value off: ", pin_in.value() == 0)

pin_out(1)
print("pin out callable 1: ", pin_in.value() == 1)

pin_out(0)
print("pin out callable 0: ", pin_in.value() == 0)


# TODO: Check how to tests the PULLUP enablement functionality.
# This is not really working because of the hardware setup? init value set?
# pull up hardware configuration not working?
# Validating pull resistors configurations and open drain mode
# pin_out.deinit()
# pin_out = Pin(pin1_name, pull=None, mode=Pin.OPEN_DRAIN)
# print("pin out with pull none initially 0 or 1: ", pin_in.value() == 0 or pin_in.value() == 1)

# pin_out.deinit()
# pin_out = Pin(pin1_name, pull=Pin.PULL_DOWN, mode=Pin.OUT)
# print("pin out with pull down initially down: ", pin_in.value() == 0)

# pin_out.deinit()
# pin_out = Pin(pin1_name, pull=Pin.PULL_UP, mode=Pin.OUT)
# print("pin out with pull up initially high: ", pin_in.value() == 1)


# Validating interrupts
def blocking_delay_ms(delay_ms):
    start = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), start) < delay_ms:
        pass


pin_in.irq(handler=lambda t: print("interrupt triggered rising"), trigger=Pin.IRQ_RISING)
pin_out.high()

blocking_delay_ms(1000)

pin_in.irq(handler=lambda t: print("interrupt triggered falling"), trigger=Pin.IRQ_FALLING)
pin_out.low()

blocking_delay_ms(1000)
