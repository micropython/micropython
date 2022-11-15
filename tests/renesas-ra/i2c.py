import os
import time

n = os.uname().machine
if "RA6M2_EK" in n:
    i2c_id = 2
elif "RA6M1_EK" in n:
    i2c_id = 0
elif ("RA4M1_CLICKER" in n) or ("RA4M1_EK" in n) or ("RA4W1_EK" in n):
    print("SKIP")
    raise SystemExit
else:
    i2c_id = 0

from machine import I2C

i2c = I2C(i2c_id)
