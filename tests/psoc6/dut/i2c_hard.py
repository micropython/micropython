### I2C

from machine import I2C
import time
import os

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    scl_master_pin = "P6_0"
    sda_master_pin = "P6_1"
    scl_slave_pin = "P9_0"
    sda_slave_pin = "P9_1"
elif "CY8CPROTO-063-BLE" in machine:
    scl_master_pin = "P6_4"
    sda_master_pin = "P6_5"

# Error missing arguments in constructor
try:
    i2c = I2C(sda=sda_master_pin, freq=400000)
except TypeError as e:
    print(e)

try:
    i2c = I2C(scl=scl_master_pin, freq=400000)
except TypeError as e:
    print(e)

i2c_master = I2C(scl=scl_master_pin, sda=sda_master_pin)
i2c_slave = I2C(scl=scl_slave_pin, sda=sda_slave_pin, addr=0x45)

# addr = i2c.scan()
# master_data = b"\x01\x00\x17"
# counter = 0

# while counter != 5:
#     i2c.writeto(addr[0], master_data)
#     time.sleep(1)
#     slave_data = i2c.readfrom(addr[0], 3)
#     print("Slave sent data: ", slave_data)
#     master_data = slave_data
#     time.sleep(1)
#     counter = counter + 1


# I2C SDA and SCL signal must be connected
# together in the same board

# master = I2C(0, scl=scl_pin, sda=sda_pin, freq=400000)
# slave = I2C(1, scl=scl_pin, sda=sda_pin, freq=400000)

# scan and find a connected slave
# i2c_slave_addr = 0x45
# scanned_addr = master.scan()
# print("i2c slave detected with address: " i2c_slave_addr == scanned_addr)

# readfrom, readfrom_into, writeto

# vector passing mode

# memory address option
