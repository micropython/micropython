"""
TouchPad

CircuitPython library for TouchPad - MixGoCE
=======================================================

Small Cabbage
20210721
dahanzimin
20210423
"""
import board
from touchio import TouchIn
class TouchPad():
    def __init__(self,pin,v=18000):
		self.pin = TouchIn(pin)
		self.v = v

    def is_touched(self):
        return self.pin.raw_value > self.v 

touch_T1 = TouchPad(board.IO4)
touch_T2 = TouchPad(board.IO5)
touch_T3 = TouchPad(board.IO6)
touch_T4 = TouchPad(board.IO7)