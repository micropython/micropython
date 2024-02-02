### I2C

from machine import I2C
from machine import SoftI2C
from machine import I2CSlave
import machine
import binascii
import time
import os

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    scl_master_pin = "P6_0"
    sda_master_pin = "P6_1"
    scl_master_soft_pin = "P9_5"
    sda_master_soft_pin = "P9_3"
    scl_slave_pin = "P9_0"
    sda_slave_pin = "P9_1"
elif "CY8CPROTO-063-BLE" in machine:
    scl_master_pin = "P6_4"
    sda_master_pin = "P6_5"

# Test hardware setup
##############################################
# I2C SDA and SCL signals from master and slave
# must be connected together (in the same board)
# and use pull-up resistors (4.7Kohm) for each line

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
i2c_soft_master = SoftI2C(scl=scl_master_soft_pin, sda=sda_master_soft_pin)


def i2c_tests(i2c_master):
    global slave_addr, i2c_slave

    # 2. Scan for slaves
    ##############################################
    addr = i2c_master.scan()
    print(addr)
    print("Found slave with address ", slave_addr, " : ", slave_addr == addr[0])

    # 3. Master to slave write
    ##############################################

    # # Configure slave
    # irq_flag = False
    # def irq_slave(arg):
    #     # print("irq : ", arg)
    #     global irq_flag
    #     irq_flag = True

    i2c_slave_rcv_buf = bytearray(8)
    i2c_slave.conf_receive_buffer(i2c_slave_rcv_buf)

    # writeto()
    master_write_data = b"\x01\x44\x17\x88\x98\x11\x34\xff"
    i2c_master.writeto(slave_addr, master_write_data)

    time.sleep_ms(100)

    # print("received buffer : ", binascii.hexlify(i2c_slave_rcv_buf))
    print("master writeto() and received by slave: ", i2c_slave_rcv_buf == master_write_data)

    # writeto_mem() #TODO

    # writevto() #TODO

    # # 4. Master to slave read
    # ##############################################
    i2c_slave_tx_buf = bytearray([0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF, 0x77, 0x44])
    i2c_slave.conf_transmit_buffer(i2c_slave_tx_buf)

    # readfrom_into()
    master_read_data = bytearray(8)
    i2c_master.readfrom_into(slave_addr, master_read_data)
    print(
        "master readfrom_into() and transmitted from slave: ", i2c_slave_tx_buf == master_read_data
    )

    # readfrom()
    i2c_slave_tx_buf = bytearray([0xAB, 0xCD])
    i2c_slave.conf_transmit_buffer(i2c_slave_tx_buf)

    master_read_data = i2c_master.readfrom(slave_addr, 2)

    print("master readfrom() and transmitted from slave: ", i2c_slave_tx_buf == master_read_data)

    # readfrom_mem_into() #TODO

    # readfrom_mem() #TODO


print("\nI2C Hardware\n")
i2c_tests(i2c_master)

print("\nI2C Software\n")
i2c_tests(i2c_soft_master)
