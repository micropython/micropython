import sys
from pyb import Pin

if "PYB" in sys.implementation._machine:
    test_pin = "X8"
else:
    test_pin = Pin.cpu.A7

p = Pin(test_pin, Pin.IN)
print(p)
print(p.name())
print(p.pin())
print(p.port())

p = Pin(test_pin, Pin.IN, Pin.PULL_UP)
p = Pin(test_pin, Pin.IN, pull=Pin.PULL_UP)
p = Pin(test_pin, mode=Pin.IN, pull=Pin.PULL_UP)
print(p)
print(p.value())

p.init(p.IN, p.PULL_DOWN)
p.init(p.IN, pull=p.PULL_DOWN)
p.init(mode=p.IN, pull=p.PULL_DOWN)
print(p)
print(p.value())

p.init(p.OUT_PP)
p.low()
print(p.value())
p.high()
print(p.value())
p.value(0)
print(p.value())
p.value(1)
print(p.value())
p.value(False)
print(p.value())
p.value(True)
print(p.value())
