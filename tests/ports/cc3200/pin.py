"""
This test need a set of pins which can be set as inputs and have no external
pull up or pull down connected.
GP12 and GP17 must be connected together
"""

from machine import Pin
import os

mch = os.uname().machine
if "LaunchPad" in mch:
    pin_map = [
        "GP24",
        "GP12",
        "GP14",
        "GP15",
        "GP16",
        "GP17",
        "GP28",
        "GP8",
        "GP6",
        "GP30",
        "GP31",
        "GP3",
        "GP0",
        "GP4",
        "GP5",
    ]
    max_af_idx = 15
elif "WiPy" in mch:
    pin_map = [
        "GP23",
        "GP24",
        "GP12",
        "GP13",
        "GP14",
        "GP9",
        "GP17",
        "GP28",
        "GP22",
        "GP8",
        "GP30",
        "GP31",
        "GP0",
        "GP4",
        "GP5",
    ]
    max_af_idx = 15
else:
    raise Exception("Board not supported!")

# test initial value
p = Pin("GP12", Pin.IN)
Pin("GP17", Pin.OUT, value=1)
print(p() == 1)
Pin("GP17", Pin.OUT, value=0)
print(p() == 0)


def test_noinit():
    for p in pin_map:
        pin = Pin(p)
        pin.value()


def test_pin_read(pull):
    # enable the pull resistor on all pins, then read the value
    for p in pin_map:
        pin = Pin(p, mode=Pin.IN, pull=pull)
    for p in pin_map:
        print(pin())


def test_pin_af():
    for p in pin_map:
        for af in Pin(p).alt_list():
            if af[1] <= max_af_idx:
                Pin(p, mode=Pin.ALT, alt=af[1])
                Pin(p, mode=Pin.ALT_OPEN_DRAIN, alt=af[1])


# test un-initialized pins
test_noinit()
# test with pull-up and pull-down
test_pin_read(Pin.PULL_UP)
test_pin_read(Pin.PULL_DOWN)

# test all constructor combinations
pin = Pin(pin_map[0])
pin = Pin(pin_map[0], mode=Pin.IN)
pin = Pin(pin_map[0], mode=Pin.OUT)
pin = Pin(pin_map[0], mode=Pin.IN, pull=Pin.PULL_DOWN)
pin = Pin(pin_map[0], mode=Pin.IN, pull=Pin.PULL_UP)
pin = Pin(pin_map[0], mode=Pin.OPEN_DRAIN, pull=Pin.PULL_UP)
pin = Pin(pin_map[0], mode=Pin.OUT, pull=Pin.PULL_DOWN)
pin = Pin(pin_map[0], mode=Pin.OUT, pull=None)
pin = Pin(pin_map[0], mode=Pin.OUT, pull=Pin.PULL_UP)
pin = Pin(pin_map[0], mode=Pin.OUT, pull=Pin.PULL_UP, drive=pin.LOW_POWER)
pin = Pin(pin_map[0], mode=Pin.OUT, pull=Pin.PULL_UP, drive=pin.MED_POWER)
pin = Pin(pin_map[0], mode=Pin.OUT, pull=Pin.PULL_UP, drive=pin.HIGH_POWER)
pin = Pin(pin_map[0], mode=Pin.OUT, drive=pin.LOW_POWER)
pin = Pin(pin_map[0], Pin.OUT, Pin.PULL_DOWN)
pin = Pin(pin_map[0], Pin.ALT, Pin.PULL_UP)
pin = Pin(pin_map[0], Pin.ALT_OPEN_DRAIN, Pin.PULL_UP)
test_pin_af()  # try the entire af range on all pins

# test pin init and printing
pin = Pin(pin_map[0])
pin.init(mode=Pin.IN)
print(pin)
pin.init(Pin.IN, Pin.PULL_DOWN)
print(pin)
pin.init(mode=Pin.OUT, pull=Pin.PULL_UP, drive=pin.LOW_POWER)
print(pin)
pin.init(mode=Pin.OUT, pull=Pin.PULL_UP, drive=pin.HIGH_POWER)
print(pin)

# test value in OUT mode
pin = Pin(pin_map[0], mode=Pin.OUT)
pin.value(0)
pin.toggle()  # test toggle
print(pin())
pin.toggle()  # test toggle again
print(pin())
# test different value settings
pin(1)
print(pin.value())
pin(0)
print(pin.value())
pin.value(1)
print(pin())
pin.value(0)
print(pin())

# test all getters and setters
pin = Pin(pin_map[0], mode=Pin.OUT)
# mode
print(pin.mode() == Pin.OUT)
pin.mode(Pin.IN)
print(pin.mode() == Pin.IN)
# pull
pin.pull(None)
print(pin.pull() == None)
pin.pull(Pin.PULL_DOWN)
print(pin.pull() == Pin.PULL_DOWN)
# drive
pin.drive(Pin.MED_POWER)
print(pin.drive() == Pin.MED_POWER)
pin.drive(Pin.HIGH_POWER)
print(pin.drive() == Pin.HIGH_POWER)
# id
print(pin.id() == pin_map[0])

# all the next ones MUST raise
try:
    pin = Pin(pin_map[0], mode=Pin.OUT, pull=Pin.PULL_UP, drive=pin.IN)  # incorrect drive value
except Exception:
    print("Exception")

try:
    pin = Pin(pin_map[0], mode=Pin.LOW_POWER, pull=Pin.PULL_UP)  # incorrect mode value
except Exception:
    print("Exception")

try:
    pin = Pin(pin_map[0], mode=Pin.IN, pull=Pin.HIGH_POWER)  # incorrect pull value
except Exception:
    print("Exception")

try:
    pin = Pin("A0", Pin.OUT, Pin.PULL_DOWN)  # incorrect pin id
except Exception:
    print("Exception")

try:
    pin = Pin(pin_map[0], Pin.IN, Pin.PULL_UP, alt=0)  # af specified in GPIO mode
except Exception:
    print("Exception")

try:
    pin = Pin(pin_map[0], Pin.OUT, Pin.PULL_UP, alt=7)  # af specified in GPIO mode
except Exception:
    print("Exception")

try:
    pin = Pin(pin_map[0], Pin.ALT, Pin.PULL_UP, alt=0)  # incorrect af
except Exception:
    print("Exception")

try:
    pin = Pin(pin_map[0], Pin.ALT_OPEN_DRAIN, Pin.PULL_UP, alt=-1)  # incorrect af
except Exception:
    print("Exception")

try:
    pin = Pin(pin_map[0], Pin.ALT_OPEN_DRAIN, Pin.PULL_UP, alt=16)  # incorrect af
except Exception:
    print("Exception")

try:
    pin.mode(Pin.PULL_UP)  # incorrect pin mode
except Exception:
    print("Exception")

try:
    pin.pull(Pin.OUT)  # incorrect pull
except Exception:
    print("Exception")

try:
    pin.drive(Pin.IN)  # incorrect drive strength
except Exception:
    print("Exception")

try:
    pin.id("ABC")  # id cannot be set
except Exception:
    print("Exception")
