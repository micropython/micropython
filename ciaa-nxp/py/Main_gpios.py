import pyb

p = pyb.Pin(8)
p.init(pyb.Pin.OUT_PP,pyb.Pin.PULL_NONE)
print(p)

while True:
	#p.value(True)
	p.high()
	print("value:"+str(p.value()))

	pyb.delay(1000)

	#p.value(False)
	p.low()
	print("value:"+str(p.value()))

	pyb.delay(1000)

