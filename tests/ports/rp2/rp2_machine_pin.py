# Test construction and re-initialization of machine.Pin objects.

from machine import Pin

pin = Pin(0, Pin.IN)

for resistor in [None, Pin.PULL_UP, Pin.PULL_DOWN, Pin.PULL_UP | Pin.PULL_DOWN]:
    pin.init(Pin.IN, pull=resistor)
    print(pin)

for slew in [0, Pin.DRIVE_FAST]:
    for strength in [Pin.DRIVE_0, Pin.DRIVE_1, Pin.DRIVE_2, Pin.DRIVE_3]:
        pin.init(Pin.OUT, pull=None, drive=strength | slew)
        print(pin)

pin.init(Pin.IN, pull=None)
