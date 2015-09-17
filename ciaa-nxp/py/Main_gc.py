import pyb
import math

class Clase:
	def __init__(self):
		pass

	def miMetodo(self,arg):
		print("Mi metodo:"+str(arg))

c = Clase()

led1 = pyb.LED(1)
led2 = pyb.LED(2)
led3 = pyb.LED(3)

switch1 = pyb.Switch(1)
switch2 = pyb.Switch(2)
switch3 = pyb.Switch(3)
switch4 = pyb.Switch(4)

counter=0
while(True):
	print('Estado de pulsadores:')
	val = switch1.value()
	print('sw1 vale:'+str(val))	
        val = switch2.value()
        print('sw2 vale:'+str(val))
        val = switch3.value()
        print('sw3 vale:'+str(val))
        val = switch4.value()
        print('sw4 vale:'+str(val))

	counter=counter+1
	print('Modificacion de leds:'+str(counter))
	print('Enciendo')
	led1.on()
	pyb.delay(100);
	print('Apago')
	led1.off()
	pyb.delay(100);

        #print('Enciendo')
        #led2.on()
        #pyb.delay(1000);
        #print('Apago')
        #led2.off()
        #pyb.delay(1000);

        #print('Enciendo')
        #led3.on()
        #pyb.delay(1000);
        #print('Apago')
        #led3.off()
        #pyb.delay(1000);

