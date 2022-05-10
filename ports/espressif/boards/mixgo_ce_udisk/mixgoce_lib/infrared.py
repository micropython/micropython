"""
Infrared Sensor

CircuitPython library for Infrared Sensor - MixGoCE
=======================================================

Small Cabbage
20210721
dahanzimin
20210423
"""

from mixgoce import version
import board
from pwmio import PWMOut

if version:#new
	def near(x, val=10000):
		from analogio import AnalogIn
		
		pwm = PWMOut(board.IO39, frequency=50000, duty_cycle=65535)
		if x == 'left':
			IR=AnalogIn(board.IO3)
			reaction = IR.value-10000  #fix
		if x == 'right':
			IR=AnalogIn(board.IO16)
			reaction = IR.value
		IR.deinit()
		pwm.deinit()
		return reaction > val
else:#old
	def near(x, val=10000):
		from digitalio import DigitalInOut, Direction

		IR = DigitalInOut(board.IO38)
		IR.direction = Direction.INPUT
		pwm = PWMOut(board.IO39, frequency=53000, duty_cycle=100)
		reaction = IR.value
		IR.deinit()
		pwm.deinit()
		return reaction