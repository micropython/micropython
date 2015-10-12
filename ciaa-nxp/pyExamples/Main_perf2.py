import pyb

led1 = pyb.LED(1)
led2 = pyb.LED(2)
led3 = pyb.LED(3)

#@micropython.native
def speed():
	for i in range(1000000):
		led1.on()
		led1.off()


speed()


