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
    scl_master_soft_pin = "P7_2"
    sda_master_soft_pin = "P0_5"
    scl_slave_pin = "P10_0"
    sda_slave_pin = "P10_1"

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

    # writeto()
    i2c_slave_rcv_buf = bytearray(8)
    i2c_slave.conf_receive_buffer(i2c_slave_rcv_buf)

    master_write_data = b"\x01\x44\x17\x88\x98\x11\x34\xff"
    i2c_master.writeto(slave_addr, master_write_data)

    time.sleep_ms(100)

    # print("received buffer : ", binascii.hexlify(i2c_slave_rcv_buf))
    print("master writeto() and received by slave: ", i2c_slave_rcv_buf == master_write_data)

    # writeto_mem()
    i2c_slave_rcv_buf = bytearray(4)
    i2c_slave.conf_receive_buffer(i2c_slave_rcv_buf)
    master_write_data = b"\x03\x44\x55"
    i2c_master.writeto_mem(slave_addr, 0x01, master_write_data)

    time.sleep_ms(100)

    print("master writeto_mem() and received by slave: ", i2c_slave_rcv_buf == b"\x01\x03\x44\x55")

    # writevto()
    i2c_slave_rcv_buf = bytearray(8)
    i2c_slave.conf_receive_buffer(i2c_slave_rcv_buf)

    master_write_data = [b"\x01\x44\x55\x23", b"\x98\x03\x44\xEE"]
    i2c_master.writevto(slave_addr, master_write_data)

    time.sleep_ms(100)

    print(
        "master writevto() and received by slave: ",
        i2c_slave_rcv_buf == b"\x01\x44\x55\x23\x98\x03\x44\xEE",
    )

    # 4. Master to slave read
    ##############################################
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

    # readfrom_mem_into()
    i2c_slave_rcv_buf = bytearray(1)
    i2c_slave.conf_receive_buffer(i2c_slave_rcv_buf)
    i2c_slave_tx_buf = bytearray([0x03, 0x44, 0x55])
    i2c_slave.conf_transmit_buffer(i2c_slave_tx_buf)
    master_read_data = bytearray(3)

    i2c_master.readfrom_mem_into(slave_addr, 0x01, master_read_data)
    print(
        "master readfrom_mem_into() and transmitted from slave: ",
        i2c_slave_tx_buf == master_read_data,
    )

    # readfrom_mem()
    i2c_slave_rcv_buf = bytearray(1)
    i2c_slave.conf_receive_buffer(i2c_slave_rcv_buf)
    i2c_slave_tx_buf = bytearray([0x43, 0x22, 0x77])
    i2c_slave.conf_transmit_buffer(i2c_slave_tx_buf)
    master_read_data = bytearray(3)

    master_read_data = i2c_master.readfrom_mem(slave_addr, 0x01, 3)
    print(
        "master readfrom_mem() and transmitted from slave: ", i2c_slave_tx_buf == master_read_data
    )

    # 5. Master to slave write and read with stop false
    ###################################################
    i2c_slave_rcv_buf = bytearray(3)
    i2c_slave.conf_receive_buffer(i2c_slave_rcv_buf)
    i2c_slave_tx_buf = bytearray([0x22, 0x11, 0x55])
    i2c_slave.conf_transmit_buffer(i2c_slave_tx_buf)
    master_read_data = bytearray(3)

    master_write_data = b"\x02\x32\x89"
    i2c_master.writeto(slave_addr, master_write_data, False)
    master_read_data = i2c_master.readfrom(slave_addr, 3, True)

    print(
        "master writeto(stop=false) and received by slave: ",
        i2c_slave_rcv_buf == master_write_data,
    )
    print("master readfrom() and transmitted from slave : ", i2c_slave_tx_buf == master_read_data)


print("\nI2C Hardware\n")
i2c_tests(i2c_master)

print("\nI2C Software\n")
i2c_tests(i2c_soft_master)

# 6. Primitive operation with SoftI2C

print("\nI2C Primitives\n")

i2c_soft_master.init(scl=scl_master_soft_pin, sda=sda_master_soft_pin)

i2c_slave_rcv_buf = bytearray(3)
i2c_slave.conf_receive_buffer(i2c_slave_rcv_buf)

# 6.1. Master to slave write
# first byte = slave = 0x45 << 1 bit = 0x8a + w (0)
master_write_data = b"\x8a\x03\x44\x55"
i2c_soft_master.start()
i2c_soft_master.write(master_write_data)
i2c_soft_master.stop()

print("master soft write based on primitives: ", i2c_slave_rcv_buf == b"\x03\x44\x55")

# 6.2. Master to slave read
# first byte = slave = 0x45 << 1 bit = 0x8a + r (1) =  0x8b
i2c_slave_tx_buf = bytearray([0x43, 0x22, 0x77])
i2c_slave.conf_transmit_buffer(i2c_slave_tx_buf)

master_read_data = bytearray(3)

i2c_soft_master.start()
i2c_soft_master.write(b"\x8b")
i2c_soft_master.readinto(master_read_data)
i2c_soft_master.stop()

print("master readinto() and transmitted from slave: ", master_read_data == b"\x43\x22\x77")

# 7. Deinit runs without error
i2c_master.deinit()
i2c_slave.deinit()
