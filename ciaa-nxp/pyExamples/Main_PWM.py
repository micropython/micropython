import pyb

print("Test PWM")

print("Setting frequency...")
pyb.PWM.set_frequency(1000)

print("Creating pwm for out0")
out0 = pyb.PWM(0)

print("Setting duty cycle to 50%")
out0.duty_cycle(50)
print("Duty cycle :"+str(out0.duty_cycle()))

out1= pyb.PWM(1)
out1.duty_cycle(25)

out10= pyb.PWM(10)
out10.duty_cycle(75)


print("Main loop")
while True:
	pyb.delay(100)


