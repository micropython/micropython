from machine import Pin


pin = Pin(2, Pin.IN)
mode = repr(pin)
Pin(2, mode=-1)
print(repr(pin) == mode)

pin.init(Pin.IN)
mode = repr(pin)
pin.init(mode=-1)
print(repr(pin) == mode)
