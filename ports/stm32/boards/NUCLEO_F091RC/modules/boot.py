# frozen boot.py
#
# By having a standard location like `boards/<board>/modules` the `boot`, `main` and other python scripts can be
# conveniently stored.
# Setting a different path for the `modules` allow easy recompilation of only the relevant files and allows for
# quick compilation of a new `firmware.*` to upload to your device
#
# In this case: `make BOARD=NUCLEO_F091RC FROZEN_MPY_DIR=boards/NUCLEO_F091RC/modules`

import pyb


def pins():
    for pin_name in dir(pyb.Pin.board):
        if not(pin_name == "__class__" or pin_name == "__name__"):
            pin = pyb.Pin(pin_name)
            print('{:10s} {:s}'.format(pin_name, str(pin)))


def af():
    for pin_name in dir(pyb.Pin.board):
        if not(pin_name == "__class__" or pin_name == "__name__"):
            pin = pyb.Pin(pin_name)
            print('{:10s} {:s}'.format(pin_name, str(pin.af_list())))


print("boot: added `af()` and `pins()` and running from flash")
