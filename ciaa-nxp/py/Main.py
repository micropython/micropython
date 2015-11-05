import pyb

print("Test ADC")

channel1 = pyb.ADC(1)
channel2 = pyb.ADC(2)
channel3 = pyb.ADC(3)

print("Main loop")
while True:
	pyb.ADC.start_conversion()
	v1 = channel1.read()
	v2 = channel2.read()
	v3 = channel3.read()
	print("value ch1:"+v1)
	print("value ch2:"+v2)
	print("value ch3:"+v3)
	pyb.delay(1000)


