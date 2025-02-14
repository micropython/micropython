import binascii
import time
import os

try:
    from machine import Pin, SPI, SPISlave
except ImportError:
    print("SKIP")
    raise SystemExit

    # Allocate pin based on board
board = os.uname().machine
if "CY8CPROTO-062-4343W" in board:
    sck_slave_pin = "P6_2"
    mosi_slave_pin = "P6_0"
    miso_slave_pin = "P6_1"
    ssel_slave_pin = "P6_3"
if "CY8CPROTO-063-BLE" in board:
    # Allocate pin based on board
    sck_slave_pin = "P9_2"
    mosi_slave_pin = "P9_0"
    miso_slave_pin = "P9_1"
    ssel_slave_pin = "P9_3"
elif "CY8CKIT-062S2-AI" in board:
    # Current not supported as
    # SPI Slave on P9_x pins seems to be not supported
    sck_master_pin = "P9_2"
    mosi_master_pin = "P9_0"
    miso_master_pin = "P9_1"
    ssel_master_pin = "P9_3"
    print("SKIP")
    raise SystemExit


def data_increase_by_one(buf):
    for i in range(len(buf)):
        buf[i] += 1


def data_decrease_by_one(buf):
    for i in range(len(buf)):
        buf[i] -= 1


# 0. Construct SPI Slave obj
spi_obj = SPISlave(
    baudrate=1000000,
    polarity=0,
    phase=0,
    bits=8,
    firstbit=SPISlave.MSB,
    ssel=ssel_slave_pin,
    sck=sck_slave_pin,
    mosi=mosi_slave_pin,
    miso=miso_slave_pin,
)

# 1. Reply for master write() read() validation
rx_buf = bytearray(8)
spi_obj.read(rx_buf)

data_increase_by_one(rx_buf)
spi_obj.write(rx_buf)

# 2. Reply for master readinto() validation
data_increase_by_one(rx_buf)
spi_obj.write(rx_buf)

# 3. Reply for write_readinto() read validation
data_increase_by_one(rx_buf)
spi_obj.write(rx_buf)

# 4. Reply write_readinto() write validation
rx_buf = bytearray(8)
spi_obj.read(rx_buf)

data_decrease_by_one(rx_buf)
spi_obj.write(rx_buf)

# 5. SPI Slave object deinit
spi_obj.deinit()
