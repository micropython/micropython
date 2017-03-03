# test pyb module on F405 MCUs

import os, pyb

if not 'STM32F405' in os.uname().machine:
    print('SKIP')
    import sys
    sys.exit()

print(pyb.freq())
print(type(pyb.rng()))
