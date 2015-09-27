'''
SD card test for the CC3200 based boards.
'''

from machine import SD
import os

mch = os.uname().machine
if 'LaunchPad' in mch:
    sd_pins = ('GP16', 'GP17', 'GP15')
elif 'WiPy' in mch:
    sd_pins = ('GP10', 'GP11', 'GP15')
else:
    raise Exception('Board not supported!')

sd = SD(pins=sd_pins)
print(sd)
sd.deinit()
print(sd)
sd.init(sd_pins)
print(sd)

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

