import pyb

print("Test switch con callbacks")

def callA(obj):
	print("evento en callback python Sw0!")
	print(obj)

def callB(obj):
        print("evento en callback python Sw1!")
        print(obj)

def callC(obj):
        print("evento en callback python Sw2!")
        print(obj)

def callD(obj):
        print("evento en callback python Sw3!")
        print(obj)



sw1 = pyb.Switch(1)
sw1.callback(callA)

sw2 = pyb.Switch(2)
sw2.callback(callB)

sw3 = pyb.Switch(3)
sw3.callback(callC)

sw4 = pyb.Switch(4)
sw4.callback(callD)

while True:
	pyb.delay(1000)


