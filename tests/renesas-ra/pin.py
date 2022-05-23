from machine import Pin
import os

n = os.uname().machine
if "RA4W1_EK" in n:
    try_pin = "P004"
    try_s = "Pin(Pin.cpu.P004, mode=Pin.IN, pull=Pin.PULL_NONE, drive=Pin.LOW_POWER)"
else:
    try_pin = "P000"
    try_s = "Pin(Pin.cpu.P000, mode=Pin.IN, pull=Pin.PULL_NONE, drive=Pin.LOW_POWER)"

p = Pin(try_pin, Pin.IN)
if str(p) == try_s:
    print("OK")
else:
    print("NG")
    print("exp: " + try_s)
    print("out: " + str(p))

p = Pin("SW1", Pin.IN, Pin.PULL_UP)
if p.mode() != 1:
    print("mode: NG")
p = Pin("SW1", Pin.IN, pull=Pin.PULL_UP)
if p.pull() != 14:
    print("pull: NG")
p = Pin("SW1", mode=Pin.IN, pull=Pin.PULL_UP)

p.init(p.IN, p.PULL_UP)
p.init(p.IN, pull=p.PULL_UP)
p.init(mode=p.IN, pull=p.PULL_UP)
print(p.value())

p.init(p.OUT)
p.init(p.OPEN_DRAIN)
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
p.off()
print(p.value())
p.on()
print(p.value())
p.off()
