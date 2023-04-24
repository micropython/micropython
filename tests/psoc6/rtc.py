# import machine
from machine import RTC
import time

rtc = RTC()
print(rtc)
# Tuple format: Year, Month, Sec, WDay*, Hour, Min, Sec, Subsec=0
# *Note: User cannot set a wrong week day value here. PSoC always calculates the right weekday using rest of the fields.
rtc.init((2023, 1, 1, 0, 0, 0, 0, 0))
print(rtc.datetime())

# Make sure that 1 second passes correctly
rtc.datetime((2023, 1, 1, 0, 0, 0, 0, 0))
print(rtc.datetime())
time.sleep_ms(1008)
print(rtc.datetime())


def set_and_print(datetime):
    rtc.datetime(datetime)
    current_datetime = rtc.now()
    print(current_datetime)
    if current_datetime == datetime:
        print("PASS")
    else:
        print("FAIL")


set_and_print((2000, 1, 1, 6, 0, 0, 0, 0))
set_and_print((2000, 1, 31, 1, 0, 0, 0, 0))
set_and_print((2000, 12, 31, 0, 0, 0, 0, 0))
set_and_print((2016, 12, 31, 6, 0, 0, 0, 0))
set_and_print((2016, 12, 31, 6, 0, 0, 0, 0))
set_and_print((2016, 12, 31, 6, 1, 0, 0, 0))
set_and_print((2016, 12, 31, 6, 12, 0, 0, 0))
set_and_print((2016, 12, 31, 6, 13, 0, 0, 0))
set_and_print((2016, 12, 31, 6, 23, 0, 0, 0))
set_and_print((2016, 12, 31, 6, 23, 1, 0, 0))
set_and_print((2016, 12, 31, 6, 23, 59, 0, 0))
set_and_print((2016, 12, 31, 6, 23, 59, 1, 0))
set_and_print((2016, 12, 31, 6, 23, 59, 59, 0))
set_and_print((2099, 12, 31, 4, 23, 59, 59, 0))


def reset_rtc():
    rtc.deinit()
    default_datetime = (2015, 1, 1, 4, 0, 0, 0, 0)
    if default_datetime == rtc.now():
        print("RESET SUCCESSFUL")
    else:
        print("RESET FAILED")


reset_rtc()
