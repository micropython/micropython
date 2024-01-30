### I2C

from machine import I2C
from machine import I2CSlave
import binascii
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

# 0. Error missing arguments in constructors
##############################################
try:
    i2c = I2C(sda=sda_master_pin, freq=400000)
except TypeError as e:
    print(e)

try:
    i2c = I2C(scl=scl_master_pin, freq=400000)
except TypeError as e:
    print(e)

try:
    i2c = I2CSlave(sda=sda_slave_pin, freq=400000)
except TypeError as e:
    print(e)

try:
    i2c = I2CSlave(scl=scl_slave_pin, freq=400000)
except TypeError as e:
    print(e)

try:
    i2c = I2CSlave(sda=sda_slave_pin, scl=scl_slave_pin, freq=400000)
except TypeError as e:
    print(e)

# 1. Construct slave and master instances
##############################################
slave_addr = 0x45
i2c_slave = I2CSlave(scl=scl_slave_pin, sda=sda_slave_pin, addr=slave_addr)

i2c_master = I2C(scl=scl_master_pin, sda=sda_master_pin)

# 2. Scan for slaves
##############################################
addr = i2c_master.scan()
print("Found slave with address ", slave_addr, " : ", slave_addr == addr[0])

# 3. Master to slave write
##############################################

# Configure slave
irq_flag = False


def irq_slave(arg):
    print("irq")
    # global irq_flag
    # irq_flag = True


i2c_rcv_buf = bytearray(8)
i2c_slave.conf_receive_buffer(i2c_rcv_buf)
i2c_slave.irq(callback=irq_slave, events=I2CSlave.WR_EVENT, priority=1)

master_data = b"\x01\x44\x17\x88\x98\x11\x34\xff"
i2c_master.writeto(slave_addr, master_data)

# # Wait for i2c slave interrupt
# while irq_flag == False:
#     pass

# print("received buffer : ", binascii.hexlify(i2c_rcv_buf))
print("master write to slave received by slave: ", i2c_rcv_buf == master_data)

# # Clear flag
# irq_flag = False


def blocking_delay_ms(delay_ms):
    start = time.ticks_ms()
    while time.ticks_diff(time.ticks_ms(), start) < delay_ms:
        pass


blocking_delay_ms(3000)

# 4. Master to slave read

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
