import os
import time

n = os.uname().machine
if ("RA6M2_EK" in n) or ("VK-RA6M5" in n):
    i2c_id = 2
elif "RA6M1_EK" in n:
    i2c_id = 0
elif ("RA4M1_CLICKER" in n) or ("RA4M1_EK" in n) or ("RA4W1_EK" in n):
    print("SKIP")
    raise SystemExit
elif ("VK-RA4W1" in n) or ("VK-RA6M3" in n):
    i2c_id = 1
else:
    i2c_id = 0

from machine import I2C

i2c = I2C(i2c_id)
