# check callback feature of the timer class

import sys
import time
from pyb import Timer

if "STM32WB" in sys.implementation._machine:
    tim_extra_id = 16
else:
    tim_extra_id = 4


# callback function that disables the callback when called
def cb1(t):
    print("cb1")
    t.callback(None)


# callback function that disables the timer when called
def cb2(t):
    print("cb2")
    t.deinit()


# callback where cb4 closes over cb3.y
def cb3(x):
    y = x

    def cb4(t):
        print("cb4", y)
        t.callback(None)

    return cb4


# create a timer with a callback, using callback(None) to stop
tim = Timer(1, freq=100, callback=cb1)
time.sleep_ms(5)
print("before cb1")
time.sleep_ms(15)

# create a timer with a callback, using deinit to stop
tim = Timer(2, freq=100, callback=cb2)
time.sleep_ms(5)
print("before cb2")
time.sleep_ms(15)

# create a timer, then set the freq, then set the callback
tim = Timer(tim_extra_id)
tim.init(freq=100)
tim.callback(cb1)
time.sleep_ms(5)
print("before cb1")
time.sleep_ms(15)

# test callback with a closure
tim.init(freq=100)
tim.callback(cb3(3))
time.sleep_ms(5)
print("before cb4")
time.sleep_ms(15)
