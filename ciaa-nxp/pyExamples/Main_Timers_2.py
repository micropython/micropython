import pyb

def callb(timer):
	print("Interval interrupt")
	print(timer)

def callbTimeout (timer):
	print("Timeout interrupt")
	print(timer)

print("Test Timers")

t1 = pyb.Timer(1)
t2 = pyb.Timer(2)

t1.interval(2000,callb)
t2.timeout(5000,callbTimeout)

t1.freq(1)
print("f t1:"+str(t1.freq()))

t1.period(204000000)
t1.prescaler(3)
print("period t1:"+str(t1.period()))
print("presc t1:"+str(t1.prescaler()))

t1.counter(0)
print("counter t1:"+str(t1.counter()))

print("src freq t1:"+str(t1.source_freq()))


while True:
	pyb.delay(100)

	#t1.counter(0)
	#t1.deinit()

