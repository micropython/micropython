# 1-Wire driver for MicroPython on ESP8266
# MIT license; Copyright (c) 2016 Damien P. George

import _rcswitch as _rc

class RCswitchError(Exception):
	pass

class RCswitch:
	'''
	RCswitch is an attempt to re-implement the arduino RCswitch library in micropython for the
	esp8266. It is currently very limited and just has the ability to send a 24bit signal at
	361MHz. The ability to send signals of difference sizes, at different speeds, and to read
	incoming signals will becoming soon (hopefully).
	'''
	def __init__(self, pin):
		self.pin = pin
		self.pin.init(pin.OPEN_DRAIN)#, pin.PULL_NONE)

##3847937
	def send(self,value):
		'''
		RCswitch.send, a method to send the supplied integer.
		
		This is currently a very limited function. It is literally just a pass through
		function. You send in an integer and that is send to the c module along with the pin
		for the c module to deal with.
		
		Args:
			value (int): The integer to send.
		'''
		return _rc.send(self.pin,value)
