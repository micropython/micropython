import sys
from pyb import Pin

# Select a pin which is unconnected.
if sys.implementation._build == "NUCLEO_H723ZG":
    test_pin_name = "F3"
else:
    test_pin_name = "A7"

test_pin = getattr(Pin.cpu, test_pin_name)


# Print Pin without id.
# This ensures an output that's consistent across MCUs.
def print_pin(pin):
    s = str(pin).split(", ")
    s[0] = s[0][0:4]
    print(", ".join(s))


p = Pin(test_pin, Pin.IN)
print_pin(p)
print(p.name() == test_pin_name)
print(p.pin() == int(test_pin_name[1]))
print(p.port() == ord(test_pin_name[0]) - ord("A"))

p = Pin(test_pin, Pin.IN, Pin.PULL_UP)
p = Pin(test_pin, Pin.IN, pull=Pin.PULL_UP)
p = Pin(test_pin, mode=Pin.IN, pull=Pin.PULL_UP)
print_pin(p)
print(p.value())

p.init(p.IN, p.PULL_DOWN)
p.init(p.IN, pull=p.PULL_DOWN)
p.init(mode=p.IN, pull=p.PULL_DOWN)
print_pin(p)
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
