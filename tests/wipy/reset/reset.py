'''
Reset script for the cc3200 boards
This is needed to force the board to reboot
with the default WLAN AP settings
'''

from machine import WDT
import time
import os

mch = os.uname().machine
if not 'LaunchPad' in mch and not 'WiPy' in mch:
    raise Exception('Board not supported!')

wdt = WDT(timeout=1000)
print(wdt)
time.sleep_ms(900)
