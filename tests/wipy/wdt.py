'''
WDT test for the CC3200 based boards
'''

from machine import WDT
import time

# test the invalid cases first
try:
    wdt = WDT(1)
except Exception:
    print("Exception")

try:
    wdt = WDT(0, 500)
except Exception:
    print("Exception")

try:
    wdt = WDT(1, timeout=2000)
except Exception:
    print("Exception")

wdt = WDT(timeout=1000)
print(wdt)

try:
    wdt = WDT(0, timeout=2000)
except Exception:
    print("Exception")

time.sleep_ms(500)
wdt.feed()
print(wdt)
time.sleep_ms(900)
wdt.feed()
print(wdt)
time.sleep_ms(950)
