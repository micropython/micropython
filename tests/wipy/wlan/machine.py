"""
machine test for the CC3200 based boards.
"""

import machine
import os
from network import WLAN

mch = os.uname().machine
if not "LaunchPad" in mch and not "WiPy" in mch:
    raise Exception("Board not supported!")

wifi = WLAN()

print(machine)
machine.idle()
print(machine.freq() == (80000000,))
print(machine.unique_id() == wifi.mac())

machine.main("main.py")

rand_nums = []
for i in range(0, 100):
    rand = machine.rng()
    if rand not in rand_nums:
        rand_nums.append(rand)
    else:
        print("RNG number repeated")
        break

for i in range(0, 10):
    machine.idle()

print("Active")

print(machine.reset_cause() >= 0)
print(machine.wake_reason() >= 0)

try:
    machine.main(123456)
except:
    print("Exception")
