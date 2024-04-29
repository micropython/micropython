import binascii
import time
import os

try:
    from machine import SPI, SPISlave
except ImportError:
    print("SKIP")
    raise SystemExit

# Allocate pin based on board
sck_master_pin = "P6_2"
mosi_master_pin = "P6_0"
miso_master_pin = "P6_1"
ssel_master_pin = "P6_3"

success = b"1"
failed = b"2"


def spi_master_configure():
    spi_obj = SPI(
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
    print("\n", spi_obj)
    return spi_obj


def _verify_test(test_case, actual_data, exp_data):
    print("rx: ", actual_data)
    if actual_data == exp_data:
        print(test_case + ": successful")
    else:
        print(test_case + ": failed")


def spi_half_duplex_communication(spi_obj, tx, rx):
    print("\n*** Half duplex communication ***")
    status = bytearray(1)
    print("1) master-->write and slave-->read")
    spi_obj.write(tx)
    print("tx: ", tx)
    status = spi_obj.read(1)
    _verify_test("master write", status, success)

    print("2) slave-->write and master-->read using readinto()")
    spi_obj.readinto(rx)
    _verify_test("master read ", rx, tx)

    print("3) slave-->write and master-->read using readinto by sending out a write_byte=0x12")
    spi_obj.readinto(rx, 0x12)
    _verify_test("master read ", rx, tx)
    status = spi_obj.read(1)
    _verify_test("write_byte received by slave correctly", status, success)


def spi_full_duplex_communication(spi_obj, tx, rx):
    print("*** Full duplex communication ***")
    exp_rx = b"\x06\x06\x05\x05"
    print("1) master-->write and slave-->read continuously")
    spi_obj.write_readinto(tx, rx)
    _verify_test("read value is same as expected", rx, exp_rx)


"""def spi_master_read_verify(spi_obj,tx,rx):
    rx = spi_obj.read(len(tx))
    print(rx)
    rx = spi_obj.read(len(tx),0x11)
    print(rx)
    print("SPI master write followed by read is successful: ", rx==tx)

def spi_master_readinto_verify(spi_obj,tx,rx):
    spi_obj.write(tx)
    print("SPI master write completed")
    rx = spi_obj.read(len(tx))
    print(rx)
    print("SPI master write followed by read is successful: ", rx==tx)"""

print("*** SPI MASTER INSTANCE ***")

spi_obj = spi_master_configure()

tx_buf = b"\x08\x01\x08\x02"
rx_buf = bytearray(4)
spi_half_duplex_communication(spi_obj, tx_buf, rx_buf)

"""tx_buf = b"\x06\x08\x05\x07"
rx_buf = bytearray(4)
spi_full_duplex_communication(spi_obj, tx_buf, rx_buf)"""
