'''
machine.Counter tests.
'''

from machine import Counter, PWM, Pin
from time import sleep_ms
import os

mch = os.uname().machine
if 'ESP32' in mch:
    ctr_pin = 21
    ctr2_pin = 23
    pwm_pin = 22
else:
    raise Exception('Board "' + mch + '" not supported!')

ctr = Counter(ctr_pin)
print(ctr)
ctr.deinit()
print(ctr)
ctr.init()
print(ctr)
ctr.deinit()
print(ctr)

print("Test with String")
try:
    ctr = Counter(str(ctr_pin))
except ValueError:
    print("Exception")

print("Test with Pin")
ctr = Counter(Pin(ctr_pin), limit=100)
print(ctr)

pwm = PWM(pwm_pin, freq=100, duty=50)
print(pwm)

sleep_ms(150)
print(ctr, ctr.value())
sleep_ms(800)
print(ctr, ctr.value())
sleep_ms(100)
print(ctr, ctr.value())
ctr.value(0)
print(ctr, ctr.value())
sleep_ms(100)
print(ctr, ctr.value())
try:
    ctr.value(100)
except ValueError:
    print("Exception")
ctr.pause()
print(ctr, ctr.value())
sleep_ms(100)
print(ctr, ctr.value())
ctr.resume()
sleep_ms(200)
print(ctr, ctr.value())
ctr.deinit()
ctr = Counter(Pin(ctr_pin), limit=200, reset=False)
print(ctr, ctr.value())
sleep_ms(100)
print(ctr, ctr.value())

print("Test decr")
ctr2 = Counter(ctr2_pin)
print(ctr2, ctr2.value())
sleep_ms(5)
ctr = Counter(Pin(ctr_pin), limit=100, direction=Counter.DOWN, edge=Counter.BOTH)
print(ctr, ctr.value())

sleep_ms(150)
print(ctr, ctr.value())
sleep_ms(800)
print(ctr, ctr.value())
sleep_ms(100)
print(ctr, ctr.value())
ctr.value(100)
print(ctr, ctr.value())
sleep_ms(100)
print(ctr, ctr.value())
try:
    ctr.value(0)
except ValueError:
    print("Exception")
ctr.deinit()




"""

sd = SD(0, pins=sd_pins)
sd = SD(id=0, pins=sd_pins)
sd = SD(0, sd_pins)

# check for memory leaks
for i in range(0, 1000):
    sd = sd = SD(0, pins=sd_pins)

# next ones should raise
try:
    sd = SD(pins=())
except Exception:
    print("Exception")

try:
    sd = SD(pins=('GP10', 'GP11', 'GP8'))
except Exception:
    print("Exception")

try:
    sd = SD(pins=('GP10', 'GP11'))
except Exception:
    print("Exception")

"""
