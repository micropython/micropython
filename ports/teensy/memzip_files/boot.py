import pyb
print("Executing boot.py")

def pins():
    for pin_name in dir(pyb.Pin.board):
        pin = pyb.Pin(pin_name)
        print('{:10s} {:s}'.format(pin_name, str(pin)))

def af():
    for pin_name in dir(pyb.Pin.board):
        pin = pyb.Pin(pin_name)
        print('{:10s} {:s}'.format(pin_name, str(pin.af_list())))
