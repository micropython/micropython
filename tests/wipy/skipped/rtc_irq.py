'''
RTC IRQ test for the CC3200 based boards.
'''

from machine import RTC
import machine
import os
import time

mch = os.uname().machine
if not 'LaunchPad' in mch and not 'WiPy' in mch:
    raise Exception('Board not supported!')

def rtc_ticks_ms(rtc):
    timedate = rtc.now()
    return (timedate[5] * 1000) + (timedate[6] // 1000)

rtc_irq_count = 0

def alarm_handler (rtc_o):
    global rtc_irq
    global rtc_irq_count
    if rtc_irq.flags() & RTC.ALARM0:
        rtc_irq_count += 1

rtc = RTC()
rtc.alarm(time=500, repeat=True)
rtc_irq = rtc.irq(trigger=RTC.ALARM0, handler=alarm_handler)

# active mode
time.sleep_ms(1000)
rtc.alarm_cancel()
print(rtc_irq_count == 2)
rtc_irq_count = 0
rtc.alarm(time=200, repeat=True)
time.sleep_ms(1000)
rtc.alarm_cancel()
print(rtc_irq_count == 5)

rtc_irq_count = 0
rtc.alarm(time=100, repeat=True)
time.sleep_ms(1000)
rtc.alarm_cancel()
print(rtc_irq_count == 10)

# deep sleep mode
rtc.alarm_cancel()
rtc_irq_count = 0
rtc.alarm(time=50, repeat=True)
rtc_irq.init(trigger=RTC.ALARM0, handler=alarm_handler, wake=machine.SLEEP | machine.IDLE)
while rtc_irq_count < 3:
    machine.sleep()
print(rtc_irq_count == 3)

# no repetition
rtc.alarm_cancel()
rtc_irq_count = 0
rtc.alarm(time=100, repeat=False)
time.sleep_ms(250)
print(rtc_irq_count == 1)

rtc.alarm_cancel()
t0 = rtc_ticks_ms(rtc)
rtc.alarm(time=500, repeat=False)
machine.sleep()
t1 = rtc_ticks_ms(rtc)
print(abs(t1 - t0 - 500) < 20)

# deep sleep repeated mode
rtc.alarm_cancel()
rtc_irq_count = 0
rtc.alarm(time=500, repeat=True)
t0 = rtc_ticks_ms(rtc)
rtc_irq = rtc.irq(trigger=RTC.ALARM0, handler=alarm_handler, wake=machine.SLEEP)
while rtc_irq_count < 3:
    machine.sleep()
    t1 = rtc_ticks_ms(rtc)
    print(abs(t1 - t0 - (500 * rtc_irq_count)) < 25)

# next ones must raise
try:
    rtc_irq = rtc.irq(trigger=10, handler=alarm_handler)
except:
    print('Exception')

try:
    rtc_irq = rtc.irq(trigger=RTC.ALARM0, wake=1789456)
except:
    print('Exception')
