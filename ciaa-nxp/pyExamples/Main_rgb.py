import pyb
import math

print("Test led RGB")


ledR = pyb.LED(4)
ledG = pyb.LED(5)
ledB = pyb.LED(6)

s0 = bytearray(100)
for i in range (0,len(s0)):
        s0[i] = 8 + int(7 * math.sin(2 * math.pi * i / (len(s0)) ) )

s1 = bytearray(100)
for i in range (0,len(s1)):
        s1[i] = 8 + int(7 * math.sin( (1/3)*math.pi +  2 * math.pi * i / (len(s1)) ) )

s2 = bytearray(100)
for i in range (0,len(s2)):
        s2[i] = 8 + int(7 * math.sin( (2/3)*math.pi +  2 * math.pi * i / (len(s2)) ) )

t=0
while True:
	pyb.delay(30)
	val = s0[t]
	ledR.intensity(val)
	#print("intensity:"+str(ledR.intensity()))

	val=s1[t]
	ledG.intensity(val)

        val=s2[t]
        ledB.intensity(val)

	t+=1
	if t==100:
		t=0

