import pyb
import math

print("Test DAC")

dac = pyb.DAC(1)

print(dac)

#dac.triangle(5000)
#dac.noise(5000)

# sawtooth
#buf = bytearray(200)
#j=0
#for i in range (0,len(buf)/2):
#	v = int(i*10)
#	buf[j+1] = (v >>  8) & 0xff
#	buf[j] = v & 0xff
#	j=j+2	

# sine
buf = bytearray(200)
j=0
for i in range (0,len(buf)/2):
        v = 512 + int(511 * math.sin(2 * math.pi * i / (len(buf)/2) ) )
        buf[j+1] = (v >>  8) & 0xff
        buf[j] = v & 0xff
        j=j+2

# output the sine-wave at 400Hz
print("sine created")

dac.write_timed(buf, 400*(int(len(buf)/2)), mode=pyb.DAC.CIRCULAR)


while True:
	print("tick")
	pyb.delay(1000)
	#dac.write(0)
	pyb.delay(1000)
	#dac.write(512)
	pyb.delay(1000)
	#dac.write(1013)

