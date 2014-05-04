import pyb
from pyb import RTC

rtc = RTC()
print(rtc)
rtc.datetime((2014, 1, 1, 1, 0, 0, 0, 0))
pyb.delay(1000)
print(rtc.datetime()[:7])
