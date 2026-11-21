# Test hardware input/output inversion via Pin(invert=...).

from machine import Pin

# Input inversion: pull-up, no external wiring required.
p = Pin(22, Pin.IN, Pin.PULL_UP)

print(p.value())
p.init(invert=True)
print(p.value())
print(p)
p.init(invert=False)
print(p.value())

# Output inversion: read back driven level on the same pin.
p.init(mode=Pin.OUT, invert=True, value=1)
print(p.value())
p.init(invert=False, value=1)
print(p.value())
print(p)
