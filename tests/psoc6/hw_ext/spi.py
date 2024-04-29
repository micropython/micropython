### SPI

from machine import SPI, SPISlave, SoftSPI
import binascii
import time
import os

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    sck_master_pin = "P6_2"
    mosi_master_pin = "P6_0"
    miso_master_pin = "P6_1"
    ssel_master_pin = "P6_3"
    sck_slave_pin = "P13_2"
    mosi_slave_pin = "P13_0"
    miso_slave_pin = "P13_1"
    ssel_slave_pin = "P13_3"
elif "CY8CPROTO-063-BLE" in machine:
    sck_master_pin = "P6_2"
    mosi_master_pin = "P6_0"
    miso_master_pin = "P6_1"
    ssel_master_pin = "P6_3"
    sck_slave_pin = "P13_2"
    mosi_slave_pin = "P13_0"
    miso_slave_pin = "P13_1"
    ssel_slave_pin = "P13_3"

master_to_slave_write = 0
master_to_slave_write_read = 1
slave_to_master_write = 2
slave_to_master_write_multi_args = 3

# Test hardware setup
####################################################
# The SCK, MOSI, MISO and SSEL pins from master
# should be connected to SCK, MISO, MOSI and SSEL
# pins of slave respectively where master and slave
# are two boards of same type.

# 0. Error missing arguments in constructors
####################################################
print("SPI Hardware Test\n")
try:
    spi = SPI(ssel=ssel_master_pin)
except Exception as e:
    print(e)

try:
    spi = SPISlave(sck=sck_master_pin)
except Exception as e:
    print(e)

try:
    spi = SoftSPI(mosi=mosi_master_pin)
except Exception as e:
    print(e)

try:
    spi = SoftSPI(miso=miso_master_pin)
except Exception as e:
    print(e)


def _validate_data(exp_rx_data, act_rx_data):
    print("data transfer is successful: ", act_rx_data == exp_rx_data)


def transfer_data_and_validate(tx_buf, rx_buf, exp_recv_data, data_transfer_dir):
    print("transffered data: ", tx_buf)
    if data_transfer_dir == master_to_slave_write:
        spi_master.write(tx_buf)
        spi_slave.read(rx_buf)
        print("received data: ", rx_buf)
        _validate_data(exp_recv_data, rx_buf)

    elif data_transfer_dir == master_to_slave_write_read:
        tx_slave_buf = b"\x00\x11\x00\x11\x00\x11\x00\x11"
        rx_slave_buf = bytearray(8)
        spi_slave.write(tx_slave_buf)
        print("slave writes: ", tx_slave_buf)
        spi_master.write_readinto(tx_buf, rx_buf)
        print("master received: ", rx_buf)
        spi_master.write_readinto(tx_buf, rx_buf)
        spi_slave.read(rx_slave_buf)
        print("slave recvd: ", rx_slave_buf)
        print("Validate slave to master data transfer")
        _validate_data(tx_slave_buf, rx_buf)
        # print("Validate master to slave data transfer")
        # _validate_data(tx_buf,rx_slave_buf)

    elif data_transfer_dir == slave_to_master_write:
        spi_slave.write(tx_buf)
        rx_buf = spi_master.read(len(rx_buf))
        print("received data by master: ", rx_buf)
        _validate_data(exp_recv_data, rx_buf)

    elif data_transfer_dir == slave_to_master_write_multi_args:
        spi_slave.write(tx_buf)
        rx = bytearray(1)
        rx_buf = spi_master.read(len(rx_buf), 0x11)
        spi_slave.read(rx)
        print("RX: ", rx)

    else:
        print("Wrong data transfer direction!")


# 1. Construct master and slave instances
##########################################
spi_master = SPI(
    baudrate=1000000,
    polarity=0,
    phase=0,
    bits=8,
    firstbit=SPI.MSB,
    ssel=ssel_master_pin,
    sck=sck_master_pin,
    mosi=mosi_master_pin,
    miso=miso_master_pin,
)
print("\n", spi_master)
spi_slave = SPISlave(
    baudrate=1000000,
    polarity=0,
    phase=0,
    bits=8,
    firstbit=SPI.MSB,
    ssel=ssel_slave_pin,
    sck=sck_slave_pin,
    mosi=mosi_slave_pin,
    miso=miso_slave_pin,
)
print(spi_slave)
# spi_soft_master = SoftSPI(sck=sck_master_pin, mosi=mosi_master_pin, miso=miso_master_pin)
# print(spi_soft_master)


# 3. Master to slave 4-bytes data transfer
###########################################
"""tx_buf = b"\x88\x88\x11\x11"
rx_buf = bytearray(4)
print("\nWriting 4-bytes data from master to slave")
transfer_data_and_validate(tx_buf, rx_buf, tx_buf, master_to_slave_write)

# 4. Slave to master 8-bytes data transfer
###########################################
tx_buf = b"\x11\x11\x88\x88\x11\x11\x88\x88"
rx_buf = bytearray(8)
print("\nWriting 8-bytes data from slave to master")
transfer_data_and_validate(tx_buf, rx_buf, tx_buf, slave_to_master_write)"""

# 7. Write from master to slave and read concurrently
######################################################
tx_buf = b"\x11\x11\x88\x88\x11\x11\x88\x88"
rx_buf = bytearray(8)
print("\nWriting 8-bytes data from slave to master")
transfer_data_and_validate(tx_buf, rx_buf, tx_buf, slave_to_master_write_multi_args)


# 5. Deinit SPI modules
##########################
spi_master.deinit()
spi_slave.deinit()

# 6. Reconfigure firstbit and transfer data from master to slave
#################################################################
"""spi_master = SPI(
    baudrate=1000000,
    polarity=0,
    phase=0,
    bits=8,
    firstbit=SPI.MSB,
    ssel=ssel_master_pin,
    sck=sck_master_pin,
    mosi=mosi_master_pin,
    miso=miso_master_pin,
)
print("\n", spi_master)
spi_slave = SPISlave(
    baudrate=1000000,
    polarity=0,
    phase=0,
    bits=8,
    firstbit=SPI.LSB,
    ssel=ssel_slave_pin,
    sck=sck_slave_pin,
    mosi=mosi_slave_pin,
    miso=miso_slave_pin,
)
print(spi_slave)
tx_buf = b"\x11\x11\x88\x88\x11\x11\x88\x88"
rx_buf = bytearray(8)
tx_exp_buf = b"\x88\x88\x11\x11\x88\x88\x11\x11"
print("\nWriting data from master to slave with firstbit as MSB (master) and LSB(slave)")
transfer_data_and_validate(tx_buf, rx_buf, tx_exp_buf, master_to_slave_write)"""
