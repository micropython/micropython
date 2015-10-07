import pyb

print("Test led RGB")


ledR = pyb.LED(6)


while True:
	pyb.delay(1000)
	#ledR.toggle()
