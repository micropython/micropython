import binascii
import time
import os

try:
    from machine import Pin, SPI, SPISlave
except ImportError:
    print("SKIP")
    raise SystemExit

    # Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    slave_write_notify_pin = "P12_3"  # Sends signals when master wants to write data
    slave_read_notify_pin = "P12_4"  # interrupt pin to check if slave is writing data
    sck_slave_pin = "P13_2"
    mosi_slave_pin = "P13_0"
    miso_slave_pin = "P13_1"
    ssel_slave_pin = "P13_3"

signal_received = False


def signal_irq(arg):
    global signal_received
    signal_received = True


pin_out = Pin(slave_write_notify_pin, mode=Pin.OUT, value=True)
pin_in = Pin(slave_read_notify_pin, Pin.IN)
pin_in.irq(handler=signal_irq, trigger=Pin.IRQ_RISING)


def _slave_ready_to_write():
    pin_out.value(0)
    time.sleep_ms(10)
    pin_out.value(1)


def _wait_for_master_signal():
    global signal_received
    while not signal_received:
        pass
    signal_received = False


def spi_slave_configure():
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

    """spi_obj = SPISlave(
        baudrate=1000000,
        polarity=0,
        phase=0,
        bits=8,
        firstbit=SPISlave.MSB,
        sck=sck_slave_pin,
        mosi=mosi_slave_pin,
        miso=miso_slave_pin,
    )"""
    return spi_obj


def _verify_test(rx, exp_data):
    if rx == exp_data:
        print("Successful")
        spi_obj.write(rx)
    else:
        print("Failed")
        spi_obj.write(failed)


def spi_half_duplex_communication(spi_obj, tx, rx):
    print("\n*** Half duplex communication ***")
    write_byte = bytearray(1)

    # 1) master-->write and slave-->read
    # print("\n1) master-->write and slave-->read")
    _wait_for_master_signal()
    spi_obj.read(rx)
    print("rx: ", rx)
    # print("slave read successful : ", rx==tx)
    _slave_ready_to_write()
    spi_obj.write(rx)

    # 2) slave-->write and master-->read"
    # print("\n2) slave-->write and master-->read using readinto()")
    tx_buf = b"\x01\x03\x05\x07\x02\x04\x06\x08"
    _slave_ready_to_write()
    spi_obj.write(tx_buf)

    # 3) slave-->write and master-->read by sending a write_byte=0x12"
    # print("\n3) slave-->write and master-->read using readinto by sending out a write_byte=0x12")
    tx = b"\x08\x06\x04\x02\x08\x06\x04\x02"
    spi_obj.write(tx)
    # spi_obj.read(write_byte)
    # print("write_byte: ", write_byte)


def spi_full_duplex_communication(spi_obj, tx, rx):
    print("*** Full duplex communication ***")
    exp_rx = b"\x08\x06\x04\x02\x07\x05\x03\x01"
    print("\n1) master-->write and slave-->read continuously")
    spi_obj.write_readinto(tx, rx)


print("\n*** SPI SLAVE INSTANCE ***")

spi_obj = spi_slave_configure()

tx_buf = b"\x08\x06\x04\x02\x08\x06\x04\x02"
rx_buf = bytearray(8)
spi_half_duplex_communication(spi_obj, tx_buf, rx_buf)

tx_buf = b"\x06\x06\x05\x05\x06\x06\x05\x05"
rx_buf = bytearray(8)
# spi_full_duplex_communication(spi_obj, tx_buf, rx_buf)

spi_obj.deinit()
