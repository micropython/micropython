# test pyb module on F411 MCUs

import os, pyb

if not 'STM32F411' in os.uname().machine:
    print('SKIP')
    import sys
    sys.exit()

print(pyb.freq())
