# import machine
from machine import RTC, Pin
import time

# pin = Pin("P13_7", Pin.OUT, value=1)

irq_counter = 0

rtc = RTC()
print(rtc)

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


def cback(event):
    global irq_counter
    # pin.value(0)
    irq_counter += 1


def check_rtc_mem_write():
    rtc.memory((2023, 1, 1, 0, 0, 0, 0, 0))
    print("rtc_memory", rtc.memory())


print("\nSetting alarm for 2000 millisecs in future\n")

rtc_irq = rtc.irq(trigger=RTC.ALARM0, handler=cback)
rtc.alarm(time=1000, repeat=True)
print("val: ", rtc.alarm_left())
time.sleep_ms(1008)
print("Alarm left is 1000: ", rtc.alarm_left())
# pin.value(1)
print("irq count = ", irq_counter)
time.sleep_ms(3008)
print("Alarm left is 1000: ", rtc.alarm_left())
print("irq count = ", irq_counter)

"""try:
    rtc.cancel()
except TypeError as e:
    print(e)"""


def set_and_print(datetime):
    rtc.datetime(datetime)
    current_datetime = rtc.now()
    print(f"current datetime is same as set datetime {datetime} :", current_datetime == datetime)


"""set_and_print((2000, 1, 1, 6, 0, 0, 0, 0))
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
set_and_print((2099, 12, 31, 4, 23, 59, 59, 0))"""

check_rtc_mem_write()


def reset_rtc():
    rtc.deinit()
    default_datetime = (2015, 1, 1, 4, 0, 0, 0, 0)
    print("\nRTC reset done: ", default_datetime == rtc.now())


# reset_rtc()
