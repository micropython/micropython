# check basic functionality of the timer class

import pyb
from pyb import Timer

tim = Timer(4)
tim = Timer(4, prescaler=100, period=200)
print(tim.prescaler())
print(tim.period())
tim.prescaler(300)
print(tim.prescaler())
tim.period(400)
print(tim.period())
