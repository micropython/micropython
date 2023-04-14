from machine import SPI, Pin, Signal
from CircuitOS import InputShift
from .Pins import *



buttons = InputShift(pin_data=Pins.INP_DATA, pin_clock=Pins.INP_SCK, pin_load=Pins.INP_PL, count=1)


def begin():


	buttons.scan()
