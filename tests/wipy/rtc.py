'''
RTC test for the CC3200 based boards.
'''

from machine import RTC
import os
import time

mch = os.uname().machine
if not 'LaunchPad' in mch and not 'WiPy' in mch:
    raise Exception('Board not supported!')

rtc = RTC()
print(rtc)
print(rtc.now()[:6])

rtc = RTC(datetime=(2015, 8, 29, 9, 0, 0, 0, None))
print(rtc.now()[:6])

rtc.deinit()
print(rtc.now()[:6])

rtc.init((2015, 8, 29, 9, 0, 0, 0, None))
print(rtc.now()[:6])
seconds = rtc.now()[5]
time.sleep_ms(1000)
print(rtc.now()[5] - seconds == 1)
seconds = rtc.now()[5]
time.sleep_ms(2000)
print(rtc.now()[5] - seconds == 2)

# initialization with shorter tuples
rtc.init((2015, 9, 19, 8, 0, 0, 0))
print(rtc.now()[5])
rtc.init((2015, 9, 19, 8, 0, 0))
print(rtc.now()[5])
rtc.init((2015, 9, 19, 8, 0))
print(rtc.now()[5])
rtc.init((2015, 9, 19, 8))
print(rtc.now()[4])
rtc.init((2015, 9, 19))
print(rtc.now()[3])

def set_and_print(datetime):
    rtc.init(datetime)
    print(rtc.now()[:6])

# make sure that setting works correctly
set_and_print((2000, 1, 1, 0, 0, 0, 0, None))
set_and_print((2000, 1, 31, 0, 0, 0, 0, None))
set_and_print((2000, 12, 31, 0, 0, 0, 0, None))
set_and_print((2016, 12, 31, 0, 0, 0, 0, None))
set_and_print((2016, 12, 31, 0, 0, 0, 0, None))
set_and_print((2016, 12, 31, 1, 0, 0, 0, None))
set_and_print((2016, 12, 31, 12, 0, 0, 0, None))
set_and_print((2016, 12, 31, 13, 0, 0, 0, None))
set_and_print((2016, 12, 31, 23, 0, 0, 0, None))
set_and_print((2016, 12, 31, 23, 1, 0, 0, None))
set_and_print((2016, 12, 31, 23, 59, 0, 50, None))
set_and_print((2016, 12, 31, 23, 59, 1, 900, None))
set_and_print((2016, 12, 31, 23, 59, 59, 100, None))
set_and_print((2048, 12, 31, 23, 59, 59, 99999, None))

rtc.init((2015, 8, 29, 9, 0, 0, 0, None))
rtc.alarm(0, 5000)
rtc.alarm(time=2000)
time.sleep_ms(1000)
left = rtc.alarm_left()
print(abs(left-1000) <= 10)
time.sleep_ms(1000)
print(rtc.alarm_left() == 0)
time.sleep_ms(100)
print(rtc.alarm_left(0) == 0)

rtc.alarm(time=1000, repeat=True)
time.sleep_ms(1500)
left = rtc.alarm_left()
print(abs(left-500) <= 15)

rtc.init((2015, 8, 29, 9, 0, 0, 0, None))
rtc.alarm(time=(2015, 8, 29, 9, 0, 45))
time.sleep_ms(1000)
left = rtc.alarm_left()
print(abs(left-44000) <= 90)
rtc.alarm_cancel()
rtc.deinit()

# next ones must raise
try:
    rtc.alarm(5000)
except:
    print('Exception')

try:
    rtc.alarm_left(1)
except:
    print('Exception')

try:
    rtc.alarm_cancel(1)
except:
    print('Exception')

try:
    rtc.alarm(5000)
except:
    print('Exception')

try:
    rtc = RTC(200000000)
except:
    print('Exception')

try:
    rtc = RTC((2015, 8, 29, 9, 0, 0, 0, None))
except:
    print('Exception')
