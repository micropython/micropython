'''
WDT test for the CC3200 based boards
'''

from pyb import WDT

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

pyb.delay(500)
wdt.feed()
print(wdt)
pyb.delay(900)
wdt.feed()
print(wdt)
pyb.delay(950)
