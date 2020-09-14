# Dev by Sonthaya Nongnuch

from machine import Pin

S1 = Pin(16, Pin.IN, Pin.PULL_UP)
S2 = Pin(14, Pin.IN, Pin.PULL_UP)

def value(pin):
    return 0 if pin.value() else 1
