# import machine
from machine import RTC
import time

IRQ_COUNTER = 0
ONE_SHOT_ALARM = 0
PERIODIC_ALARM = 1

# Tuple format: Year, Month, Sec, WDay*, Hour, Min, Sec, Subsec=0
# *Note: User cannot set a wrong week day value here. PSoC always calculates the right weekday using rest of the fields.
initial_dtime = (2023, 1, 1, 0, 0, 0, 0, 0)


def cback(event):
    global IRQ_COUNTER
    IRQ_COUNTER += 1


def check_rtc_mem_write():
    rtc.memory((2023, 1, 1, 0, 0, 0, 0, 0))
    print("\ndatetime to be retrieved post soft-reset : ", rtc.memory())


def reset_rtc():
    rtc.deinit()
    default_datetime = (2015, 1, 1, 4, 0, 0, 0, 0)
    print("\nRTC reset done: ", default_datetime == rtc.now())


def set_alarm_ms(rtc, alarm_type, period_ms):
    rtc.datetime(initial_dtime)
    rtc_irq = rtc.irq(handler=cback)
    rtc.alarm(period_ms, repeat=alarm_type)


def set_alarm_datetime(rtc, alarm_type, datetime):
    rtc.datetime(initial_dtime)
    rtc_irq = rtc.irq(trigger=RTC.ALARM0, handler=cback)
    rtc.alarm(datetime, repeat=alarm_type)


def wait_for_cback(timeout, exp_counter):
    global IRQ_COUNTER
    start = time.ticks_ms()
    while IRQ_COUNTER < exp_counter:
        time.sleep_ms(5)
        if time.ticks_diff(time.ticks_ms(), start) > cback_call_wait_time:
            break


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
timeout = 1000
cback_call_wait_time = timeout + 200
set_alarm_ms(rtc, PERIODIC_ALARM, timeout)
print("Alarm period set to (ms): ", rtc.alarm_left())
wait_for_cback(cback_call_wait_time, 1)
print("Alarm expired : ", IRQ_COUNTER == 1)
print("Alarm set again...")
wait_for_cback(cback_call_wait_time, 2)
print("Alarm expired : ", IRQ_COUNTER == 2)
rtc.cancel()
wait_for_cback(cback_call_wait_time, 2)
print("Alarm cancelled successfully : ", IRQ_COUNTER == 2)
IRQ_COUNTER = 0

print("\n2. Setting one-shot short alarm to be triggered in few ms in future")
timeout = 1000
cback_call_wait_time = timeout + 500
set_alarm_ms(rtc, ONE_SHOT_ALARM, timeout)
wait_for_cback(cback_call_wait_time, 1)
print("Alarm expired : ", 0 == rtc.alarm_left())
print("Entered Cback :", IRQ_COUNTER == 1)
IRQ_COUNTER = 0

print("\n3. Setting one-shot alarm to be triggered at specified date-time")
timeout = 1
cback_call_wait_time = timeout * 1000 + 500
set_alarm_datetime(rtc, ONE_SHOT_ALARM, (2023, 1, 1, 0, 0, timeout, 0, 0))
wait_for_cback(cback_call_wait_time, 1)
print("Alarm expired : ", 0 == rtc.alarm_left())
print("Entered Cback :", IRQ_COUNTER == 1)
IRQ_COUNTER = 0

print("\n4. Setting periodic alarm to be triggered at specified date-time should fail")
try:
    set_alarm_datetime(rtc, PERIODIC_ALARM, (2023, 1, 1, 0, 0, 0, 1, 0))  # Should fail
except ValueError as e:
    print(e)

IRQ_COUNTER = 0

rtc1 = RTC()
print("\n5.RTC constructor return singleton: ", rtc1 == rtc)

reset_rtc()

check_rtc_mem_write()
