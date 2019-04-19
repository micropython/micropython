# MicroPython I2C hardware test.
# Set which I2C ports to test in "available_I2Cs".
# Connect each I2C port to an I2C device with external pullup resistors if not on board.
# A single I2C device can be moved from port to port.
# I2C.scan() should find the device on each port.

# (C) 2019 Chris Mason
# Licenced under the terms of the MicroPython MIT licence.
# https://github.com/micropython/micropython/blob/master/LICENSE

import pyb
from pyb import I2C

# -----------------------------------------------
available_I2Cs = [1, 2, 3]
# -----------------------------------------------

i2cs = []

for i in range(0, max(available_I2Cs)):
    i2cs.append('none')
    if (i+1) in available_I2Cs:
        i2cs[i] = I2C(i+1, I2C.MASTER,  baudrate=400000)

try:
    while True:
        for i in available_I2Cs:
            print('I2C' + str(i), i2cs[i-1].scan())
            pyb.delay(100)
        print('------------------')
finally:
    print('FAIL, I2C ', i, '\n')