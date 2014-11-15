# test hal errors

import pyb

i2c = pyb.I2C(2, pyb.I2C.MASTER)
try:
    i2c.recv(1, 1)
except OSError as e:
    print(repr(e))

can = pyb.CAN(1, pyb.CAN.NORMAL)
try:
    can.send('1', 1, timeout=50)
except OSError as e:
    print(repr(e))
