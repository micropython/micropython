import pyb
from pyb import RTC

rtc = RTC()
print(rtc)

# make sure that 1 second passes correctly
rtc.datetime((2014, 1, 1, 1, 0, 0, 0, 0))
pyb.delay(1001)
print(rtc.datetime()[:7])

def set_and_print(datetime):
    rtc.datetime(datetime)
    print(rtc.datetime()[:7])

# make sure that setting works correctly
set_and_print((2000, 1, 1, 1, 0, 0, 0, 0))
set_and_print((2000, 1, 31, 1, 0, 0, 0, 0))
set_and_print((2000, 12, 31, 1, 0, 0, 0, 0))
set_and_print((2016, 12, 31, 1, 0, 0, 0, 0))
set_and_print((2016, 12, 31, 7, 0, 0, 0, 0))
set_and_print((2016, 12, 31, 7, 1, 0, 0, 0))
set_and_print((2016, 12, 31, 7, 12, 0, 0, 0))
set_and_print((2016, 12, 31, 7, 13, 0, 0, 0))
set_and_print((2016, 12, 31, 7, 23, 0, 0, 0))
set_and_print((2016, 12, 31, 7, 23, 1, 0, 0))
set_and_print((2016, 12, 31, 7, 23, 59, 0, 0))
set_and_print((2016, 12, 31, 7, 23, 59, 1, 0))
set_and_print((2016, 12, 31, 7, 23, 59, 59, 0))
set_and_print((2099, 12, 31, 7, 23, 59, 59, 0))
