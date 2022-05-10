"""
Button

CircuitPython library for Button - MixGoCE
=======================================================
Small Cabbage
20210721
dahanzimin
20210423
"""

import time
import board
from digitalio import DigitalInOut, Direction, Pull

class Button():
	
	def __init__(self, pin):
		self.pin = DigitalInOut(pin)
		self.pin.direction = Direction.INPUT
		self.pin.pull = Pull.UP
		self.flag = True

	def is_pressed(self):
		return self.pin.value == False

	def get_presses(self, delay=1):
		last_time,  presses = time.monotonic(), 0
		while time.monotonic()< last_time + delay:
			time.sleep(0.05)
			if self.was_pressed():
				presses += 1
		return presses

	def was_pressed(self):
		if self.pin.value != self.flag:
			self.flag = self.pin.value
			time.sleep(0.01)
			if self.flag == False:
				return True
			else:
				return False

button_A1 = Button(board.IO14)
button_A2 = Button(board.IO21)
button_A3 = Button(board.IO36)
button_A4 = Button(board.IO37)
button_B1 = Button(board.IO0)
button_B2 = Button(board.IO35)