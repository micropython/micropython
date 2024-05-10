# import machine
from machine import RTC
import time


def cback(event):
    if event == (event & RTC.ALARM0):
        print("\nEvent triggered\n")


rtc = RTC()
rtc.alarm(time=500, repeat=True)
rtc_irq = rtc.irq(trigger=RTC.ALARM0, handler=cback)

# Tuple format: Year, Month, Sec, WDay*, Hour, Min, Sec, Subsec=0
# *Note: User cannot set a wrong week day value here. PSoC always calculates the right weekday using rest of the fields.
initial_dtime = (2023, 1, 1, 0, 0, 0, 0, 0)
rtc.init(initial_dtime)
print("\nRTC init successful: \t", rtc.datetime() == initial_dtime)

# Make sure that 1 second passes correctly
exp_dtime = (2023, 1, 1, 0, 0, 0, 1, 0)
rtc.datetime((2023, 1, 1, 0, 0, 0, 0, 0))
time.sleep_ms(1008)
print("\ndatetime is accurate: \t", rtc.now() == exp_dtime)


def set_and_print(datetime):
    rtc.datetime(datetime)
    current_datetime = rtc.now()
    # print(current_datetime)
    print(f"current datetime is same as set datetime {datetime} :", current_datetime == datetime)


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


# rtc.alarm(time=200, repeat=False)
# rtc_irq = rtc.irq(trigger=RTC.ALARM0, handler=cback)


def reset_rtc():
    rtc.deinit()
    default_datetime = (2015, 1, 1, 4, 0, 0, 0, 0)
    print("\nRTC reset done: ", default_datetime == rtc.now())


reset_rtc()
