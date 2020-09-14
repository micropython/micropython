# Dev by Sonthaya Nongnuch

from machine import Pin

last_value = 0

def value(x):
    global last_value
    Pin(25, Pin.OUT).value(0 if x else 1)
    last_value = x

def on():
    value(1)

def off():
    value(0)

def toggle():
    global last_value
    last_value = 0 if last_value else 1
    value(last_value)
