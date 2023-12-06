import os
from machine import Pin
import machine

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    pin1_name = "P13_7"
    pin2_name = "P13_6"
elif "CY8CPROTO-063-BLE" in machine:
    pin1_name = "P6_3"
    pin2_name = "P6_2"

# Pin out and pin in must be connected
# together in the board

pin_out = Pin(pin1_name)
pin_out.init(Pin.OUT)
pin_in = Pin(pin2_name, Pin.IN)

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

pin_in.irq(handler=lambda t:print("Interrupt triggered"),trigger=Pin.IRQ_RISING|Pin.IRQ_FALLING)
pin_out.high()
