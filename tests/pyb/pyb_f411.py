# test pyb module on F411 MCUs

import os, pyb

if not "STM32F411" in os.uname().machine:
    print("SKIP")
    raise SystemExit

print(pyb.freq())
