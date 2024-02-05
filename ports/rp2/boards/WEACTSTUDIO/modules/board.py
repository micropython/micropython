from machine import Pin

led = Pin(25, Pin.OUT, value=0)
key = Pin(23, Pin.IN, Pin.PULL_UP)
