import pyb
import machine
try:
    # This will run boot.py from /sd if it exists, otherwise it will run the
    # code from within the except portion.
    import os
    os.stat('/sd/boot.py')
    execfile('/sd/boot.py')
except:

    print("Executing boot.py")

    def pins():
        for pin_name in dir(pyb.Pin.board):
            pin = pyb.Pin(pin_name)
            print('{:10s} {:s}'.format(pin_name, str(pin)))

    def af():
        for pin_name in dir(pyb.Pin.board):
            pin = pyb.Pin(pin_name)
            print('{:10s} {:s}'.format(pin_name, str(pin.af_list())))
