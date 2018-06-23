# show_pins.py

import pyb


def pins():
    for pin_name in dir(pyb.Pin.board):
        if not(pin_name == "__class__" or pin_name == "__name__"):
            pin = pyb.Pin(str(pin_name))
            print('{:10s} {:s}'.format(pin_name, str(pin)))


def af():
    for pin_name in dir(pyb.Pin.board):
        if not(pin_name == "__class__" or pin_name == "__name__"):
            pin = pyb.Pin(pin_name)
            print('{:10s} {:s}'.format(pin_name, str(pin.af_list())))
