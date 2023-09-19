#### SoftI2C
import os
from machine import SoftI2C

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    scl_pin = "P6_0"
    sda_pin = "P6_1"
elif "CY8CPROTO-063-BLE" in machine:
    print("SKIP")
    raise SystemExit

si2c = SoftI2C(scl=scl_pin, sda=sda_pin, freq=400000)
print(si2c)

print(si2c.scan())
