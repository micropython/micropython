import binascii
import time
import os

try:
    from machine import SPI, SPISlave
except ImportError:
    print("SKIP")
    raise SystemExit

sck_slave_pin = "P13_2"
mosi_slave_pin = "P13_0"
miso_slave_pin = "P13_1"
ssel_slave_pin = "P13_3"

success = b"1"
failed = b"2"


def spi_slave_configure():
    spi_obj = SPISlave(
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
    print("\n", spi_obj)
    return spi_obj


def _verify_test(rx, exp_data):
    if rx == exp_data:
        print("Successful")
        spi_obj.write(success)
    else:
        print("Failed")
        spi_obj.write(failed)


def spi_half_duplex_communication(spi_obj, tx, rx):
    print("\n***Half duplex communication ***")
    write_byte = "0x12"
    write_byte_status = bytearray(1)
    print("1) master-->write and slave-->read")
    # 1) master-->write and slave-->read
    spi_obj.read(rx)
    print("rx: ", rx)
    exp_data = tx
    _verify_test(rx, exp_data)

    # 2) slave-->write and master-->read"
    print("2) slave-->write and master-->read using readinto()")
    spi_obj.write(tx)

    # 3) slave-->write and master-->read by sending a write_byte=0x12"
    print("3) slave-->write and master-->read using readinto by sending out a write_byte=0x12")
    spi_obj.write(tx)
    spi_obj.read(write_byte_status)
    _verify_test(write_byte_status, b"0x12")


def spi_full_duplex_communication(spi_obj, tx, rx):
    print("*** Full duplex communication ***")
    exp_rx = b"\x06\x08\x05\x07"
    print("1) master-->write and slave-->read continuously")
    spi_obj.write_readinto(tx, rx)
    print(rx)
    # _verify_test("")


print("*** SPI SLAVE INSTANCE ***")

spi_obj = spi_slave_configure()

tx_buf = b"\x08\x01\x08\x02"
rx_buf = bytearray(4)
spi_half_duplex_communication(spi_obj, tx_buf, rx_buf)

"""tx_buf = b"\x06\x06\x05\x05"
rx_buf = bytearray(4)
spi_full_duplex_communication(spi_obj, tx_buf, rx_buf)"""
