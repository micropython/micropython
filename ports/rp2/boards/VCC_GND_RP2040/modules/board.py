from machine import Pin

led = Pin(25, Pin.OUT)
key = Pin(24, Pin.IN, Pin.PULL_UP)
