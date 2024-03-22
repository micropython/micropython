import os
import time

n = os.uname().machine
if "EK-RA6M2" in n:
    i2c_id = 2
elif "EK-RA6M1" in n:
    i2c_id = 0
elif ("RA4M1 CLICKER" in n) or ("EK-RA4M1" in n) or ("EK-RA4W1" in n):
    print("SKIP")
    raise SystemExit
else:
    i2c_id = 0

from machine import I2C

i2c = I2C(i2c_id)
