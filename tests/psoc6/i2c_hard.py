### I2C
import os
from machine import I2C

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    scl_pin = "P6_0"
    sda_pin = "P6_1"
elif "CY8CPROTO-063-BLE" in machine:
    scl_pin = "P6_4"
    sda_pin = "P6_5"

i2c = I2C(id=0, scl=scl_pin, sda=sda_pin, freq=400000)

print(i2c.scan())
