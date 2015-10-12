import pyb
import math

print("Test Timers")


t2 = pyb.Timer(2)

print(t2)

t2.init(freq=100)

while True:
	pyb.delay(1000)

