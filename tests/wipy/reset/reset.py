'''
Reset script for the cc3200 boards
This is needed to force the board to reboot
with the default WLAN AP settings
'''

from pyb import WDT
import time

wdt = WDT(timeout=1000)
print(wdt)
time.sleep_ms(900)
