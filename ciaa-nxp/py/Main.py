import pyb
import math

def callb(timer):
	print("Timer tick")
	print(timer)


print("Test Timers")


t2 = pyb.Timer(2)

print(t2)

t2.init(freq=1)
t2.callback(callb)


while True:
	pyb.delay(1000)

