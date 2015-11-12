import pyb

@micropython.native
def performanceTest():
	d = 10000
	print("start. Wait for %d milis" % d)
	millis = pyb.millis
	endTime = millis() + d
	count = 0
	while millis() < endTime:
		count += 1
	print("Count: ", count)

while(True):
	performanceTest()
