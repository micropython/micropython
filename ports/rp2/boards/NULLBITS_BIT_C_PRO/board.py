from machine import Pin, Signal

led_red = Signal("LED_RED", Pin.OUT, invert=True, value=0)
led_green = Signal("LED_GREEN", Pin.OUT, invert=True, value=0)
led_blue = Signal("LED_BLUE", Pin.OUT, invert=True, value=0)

del Pin
