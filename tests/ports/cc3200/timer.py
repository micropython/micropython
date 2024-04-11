"""
Timer test for the CC3200 based boards.
"""

from machine import Timer
import os
import time

mch = os.uname().machine
if "LaunchPad" in mch:
    pwm_pin = "GP24"
elif "WiPy" in mch:
    pwm_pin = "GP24"
else:
    raise Exception("Board not supported!")

for i in range(4):
    tim = Timer(i, mode=Timer.PERIODIC)
    print(tim)
    ch = tim.channel(Timer.A, freq=5)
    print(ch)
    ch = tim.channel(Timer.B, freq=5)
    print(ch)
    tim = Timer(i, mode=Timer.ONE_SHOT)
    print(tim)
    ch = tim.channel(Timer.A, freq=50)
    print(ch)
    ch = tim.channel(Timer.B, freq=50)
    print(ch)
    tim = Timer(i, mode=Timer.PWM)
    print(tim)
    ch = tim.channel(Timer.A, freq=50000, duty_cycle=2000, polarity=Timer.POSITIVE)
    print(ch)
    ch = tim.channel(Timer.B, freq=50000, duty_cycle=8000, polarity=Timer.NEGATIVE)
    print(ch)
    tim.deinit()
    print(tim)

for i in range(4):
    tim = Timer(i, mode=Timer.PERIODIC)
    tim.deinit()


class TimerTest:
    def __init__(self):
        self.tim = Timer(0, mode=Timer.PERIODIC)
        self.int_count = 0

    def timer_isr(self, tim_ch):
        self.int_count += 1


timer_test = TimerTest()
ch = timer_test.tim.channel(Timer.A, freq=5)
print(ch.freq() == 5)
ch.irq(handler=timer_test.timer_isr, trigger=Timer.TIMEOUT)
time.sleep_ms(1001)
print(timer_test.int_count == 5)

ch.freq(100)
timer_test.int_count = 0
time.sleep_ms(1001)
print(timer_test.int_count == 100)

ch.freq(1000)
time.sleep_ms(1500)
timer_test.int_count = 0
time.sleep_ms(2000)
print(timer_test.int_count == 2000)

timer_test.tim.deinit()
timer_test.tim.init(mode=Timer.ONE_SHOT)
ch = timer_test.tim.channel(Timer.A, period=100000)
ch.irq(handler=timer_test.timer_isr, trigger=Timer.TIMEOUT)
timer_test.int_count = 0
time.sleep_ms(101)
print(timer_test.int_count == 1)
time.sleep_ms(101)
print(timer_test.int_count == 1)
timer_test.tim.deinit()
print(timer_test.tim)

# 32 bit modes
tim = Timer(0, mode=Timer.PERIODIC, width=32)
ch = tim.channel(Timer.A | Timer.B, period=5000000)

# check for memory leaks...
for i in range(1000):
    tim = Timer(0, mode=Timer.PERIODIC)
    ch = tim.channel(Timer.A, freq=5)

# next ones must fail
try:
    tim = Timer(0, mode=12)
except:
    print("Exception")

try:
    tim = Timer(4, mode=Timer.ONE_SHOT)
except:
    print("Exception")

try:
    tim = Timer(0, mode=Timer.PWM, width=32)
except:
    print("Exception")

tim = Timer(0, mode=Timer.PWM)

try:
    ch = tim.channel(TIMER_A | TIMER_B, freq=10)
except:
    print("Exception")

try:
    ch = tim.channel(TIMER_A, freq=4)
except:
    print("Exception")
