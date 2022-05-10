"""
Led

CircuitPython library for Led - MixGoCE
=======================================================

Small Cabbage
20210721
dahanzimin
20210423
"""
import board
from pwmio import PWMOut

class Led(object):  #fix
    def __init__(self, pin):
		self.pin = PWMOut(pin, frequency=5000, duty_cycle=65535)

    def setbrightness(self, val):
        self.pin.duty_cycle = 65535 - val

    def setonoff(self, val):
        if val == 1:
            self.pin.duty_cycle = 0
        elif val == 0:
            self.pin.duty_cycle = 65535
        else:
            self.pin.duty_cycle = 65535 - self.pin.duty_cycle

    def getonoff(self):
        return (65535 - self.pin.duty_cycle) // 65535

led_L1 = Led(board.IO33)
led_L2 = Led(board.IO34)