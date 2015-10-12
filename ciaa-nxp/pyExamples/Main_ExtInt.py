import pyb

def callBack(line):
	print("Pin Interrupt!")
	print("Line = ",line)

p = pyb.Pin(0)
p.init(pyb.Pin.OUT_PP,pyb.Pin.PULL_NONE)
print(p)
int = pyb.ExtInt(p,pyb.ExtInt.IRQ_RISING,pyb.Pin.PULL_NONE,callBack)
print(int)

int.disable()
int.enable()

switch1 = pyb.Switch(1)

while True:
	pyb.delay(1000)
	print("tick")
	if switch1.switch():
		int.swint()


