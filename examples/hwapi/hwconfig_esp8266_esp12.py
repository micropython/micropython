from machine import Pin, Signal

# ESP12 module as used by many boards
# Blue LED on pin 2, active low (inverted)
LED = Signal(Pin(2, Pin.OUT), inverted=True)
