# import machine
from machine import RTC
import time

irq_counter = 0
one_shot_alarm = 0
periodic_alarm = 1

# Tuple format: Year, Month, Sec, WDay*, Hour, Min, Sec, Subsec=0
# *Note: User cannot set a wrong week day value here. PSoC always calculates the right weekday using rest of the fields.
initial_dtime = (2023, 1, 1, 0, 0, 0, 0, 0)


def cback(event):
    global irq_counter
    irq_counter += 1


def check_rtc_mem_write():
    rtc.memory((2023, 1, 1, 0, 0, 0, 0, 0))
    print("\ndatetime to be retrieved post soft-reset : ", rtc.memory())


def reset_rtc():
    rtc.deinit()
    default_datetime = (2015, 1, 1, 4, 0, 0, 0, 0)
    print("\nRTC reset done: ", default_datetime == rtc.now())


def set_alarm_ms(rtc, alarm_type, period_ms):
    rtc.datetime(initial_dtime)
    rtc_irq = rtc.irq(trigger=RTC.ALARM0, handler=cback)
    rtc.alarm(period_ms, repeat=alarm_type)


def set_alarm_datetime(rtc, alarm_type, datetime):
    rtc.datetime(initial_dtime)
    rtc_irq = rtc.irq(trigger=RTC.ALARM0, handler=cback)
    rtc.alarm(datetime, repeat=alarm_type)


print("*** RTC Tests ***")
rtc = RTC()

rtc.init(initial_dtime)
print("\nRTC init successful: \t", rtc.datetime() == initial_dtime)

# Make sure that 1 second passes correctly
exp_dtime = (2023, 1, 1, 0, 0, 0, 1, 0)
rtc.datetime((2023, 1, 1, 0, 0, 0, 0, 0))
time.sleep_ms(1008)
print("\ndatetime is accurate: \t", rtc.now() == exp_dtime)


print("\n1. Setting periodic short alarm to be triggered repeatedly in few ms in future")
set_alarm_ms(rtc, periodic_alarm, 3000)
print("Alarm period set to (ms): ", rtc.alarm_left())
time.sleep_ms(3008)
print("Alarm expired : ", irq_counter == 1)
print("Alarm set again...")
time.sleep_ms(3008)
print("Alarm expired : ", irq_counter == 2)
rtc.cancel()
time.sleep_ms(3008)
print("Alarm cancelled successfully : ", irq_counter == 2)
irq_counter = 0

print("\n2. Setting one-shot short alarm to be triggered in few ms in future")
set_alarm_ms(rtc, one_shot_alarm, 1000)
time.sleep_ms(1008)
print("Alarm expired : ", 0 == rtc.alarm_left())
print("Entered Cback :", irq_counter == 1)
irq_counter = 0

print("\n3. Setting one-shot alarm to be triggered at specified date-time")
set_alarm_datetime(rtc, one_shot_alarm, (2023, 1, 1, 0, 0, 1, 0, 0))
time.sleep_ms(1008)
print("Alarm expired : ", 0 == rtc.alarm_left())
print("Entered Cback :", irq_counter == 1)
irq_counter = 0

print("\n4. Setting periodic alarm to be triggered at specified date-time should fail")
try:
    set_alarm_datetime(rtc, periodic_alarm, (2023, 1, 1, 0, 0, 0, 1, 0))  # Should fail
except ValueError as e:
    print(e)

irq_counter = 0

reset_rtc()

check_rtc_mem_write()
