### I2C

from machine import I2C
import time
import os

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    scl_pin = "P6_0"
    sda_pin = "P6_1"
elif "CY8CPROTO-063-BLE" in machine:
    scl_pin = "P6_4"
    sda_pin = "P6_5"

i2c = I2C(0,scl=scl_pin,sda=sda_pin,freq=400000)
addr = i2c.scan()
master_data = b'\x01\x00\x17'
counter = 0

while(counter!=5):
    i2c.writeto(addr[0], master_data)
    time.sleep(1)
    slave_data = i2c.readfrom(addr[0], 3)
    print("Slave sent data: ", slave_data)
    master_data = slave_data
    time.sleep(1)
    counter = counter+1
