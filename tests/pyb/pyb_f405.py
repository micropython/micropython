# test pyb module on F405 MCUs

import os, pyb

if not 'STM32F405' in os.uname().machine:
    print('SKIP')
    raise SystemExit

print(pyb.freq())
print(type(pyb.rng()))

# test HAL error specific to F405
i2c = pyb.I2C(2, pyb.I2C.MASTER)
try:
    i2c.recv(1, 1)
except OSError as e:
    print(repr(e))

