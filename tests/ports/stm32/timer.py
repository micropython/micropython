# check basic functionality of the timer class

import sys
from pyb import Timer

if "STM32WB" in sys.implementation._machine:
    tim_id = 16
else:
    tim_id = 4

tim = Timer(tim_id)
tim = Timer(tim_id, prescaler=100, period=200)
print(tim.prescaler())
print(tim.period())
tim.prescaler(300)
print(tim.prescaler())
tim.period(400)
print(tim.period())

# Setting and printing frequency
tim = Timer(2, freq=100)
print(tim.freq())
tim.freq(0.001)
print("{:.3f}".format(tim.freq()))
