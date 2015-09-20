import pyb

print("Test switch con callbacks")

def callB(obj):
	print("evetno en callback python!")


sw1 = pyb.Switch(1)
sw1.callback(callB)

while True:
	value = sw1.value()
	print("sw1:"+str(value))
	pyb.delay(1000)


