# test pyb module on F405 MCUs

import os, pyb

if not 'STM32F405' in os.uname().machine:
    print('SKIP')
    raise SystemExit

print(pyb.freq())
print(type(pyb.rng()))
