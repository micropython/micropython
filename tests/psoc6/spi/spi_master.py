import binascii
import time
import os

try:
    from machine import Pin, SPI, SPISlave, SoftSPI
except ImportError:
    print("SKIP")
    raise SystemExit

machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    master_write_notify_pin = "P12_4"  # Sends signals when master wants to write data
    master_read_notify_pin = "P12_3"  # interrupt pin to check if slave is writing data
    # Allocate pin based on board
    sck_master_pin = "P6_2"
    mosi_master_pin = "P6_0"
    miso_master_pin = "P6_1"
    ssel_master_pin = "P6_3"
if "CY8CPROTO-063-BLE" in machine:
    master_write_notify_pin = "P9_4"  # Sends signals when master wants to write data
    master_read_notify_pin = "P9_5"  # interrupt pin to check if slave is writing data
    # Allocate pin based on board
    sck_master_pin = "P9_2"
    mosi_master_pin = "P9_0"
    miso_master_pin = "P9_1"
    ssel_master_pin = "P9_3"


print("\n*** SPI MASTER INSTANCE ***\n")

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

signal_received = False


def signal_irq(arg):
    global signal_received
    signal_received = True


pin_out = Pin(master_write_notify_pin, mode=Pin.OUT, value=True)
pin_in = Pin(master_read_notify_pin, Pin.IN)
pin_in.irq(handler=signal_irq, trigger=Pin.IRQ_RISING)


def _master_ready_to_write():
    pin_out.value(0)
    time.sleep_ms(10)
    pin_out.value(1)


def _wait_for_slave_signal():
    global signal_received
    while not signal_received:
        pass
    signal_received = False


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
    return spi_obj


def soft_spi_master_configure():
    spi_obj = SoftSPI(
        baudrate=1000000,
        polarity=0,
        phase=0,
        bits=8,
        firstbit=SPI.MSB,
        sck=sck_master_pin,
        mosi=mosi_master_pin,
        miso=miso_master_pin,
    )
    return spi_obj


def _verify_test(test_case, actual_data, exp_data):
    print("rx: ", actual_data)
    if actual_data == exp_data:
        print(test_case + ": successful")
    else:
        print(test_case + ": failed")


def spi_half_duplex_communication(spi_obj, tx, rx):
    print("\n*** Half duplex communication ***")
    print("\nTest Case 1: master-->write and slave-->read")
    # _master_ready_to_write()
    spi_obj.write(tx)
    print("Master wrote (tx): \t", tx)
    # _wait_for_slave_signal()
    rx = spi_obj.read(8)
    print("Slave returned (rx): \t", rx)
    print("Test case successful : \t", rx == tx)

    print("\nTest Case 2: slave-->write and master-->read using readinto()")
    rx = bytearray(8)
    # _wait_for_slave_signal()
    spi_obj.readinto(rx)
    exp = b"\x01\x03\x05\x07\x02\x04\x06\x08"
    print("Slave wrote (rx): \t", rx)
    print("Master expects (exp): \t", exp)
    print("Test case successful : \t", rx == exp)

    print(
        "\nTest Case 3: slave-->write and master-->read using readinto by sending out a write_byte=5"
    )
    exp = b"\x08\x06\x04\x02\x08\x06\x04\x02"
    spi_obj.readinto(rx, 5)
    print("Slave wrote (rx): \t", rx)
    print("Master expects (exp): \t", exp)
    print("Test case successful : \t", rx == exp)


def spi_full_duplex_communication(spi_obj, tx, rx):
    print("*** Full duplex communication ***")
    exp_rx = b"\x06\x06\x05\x05\x06\x06\x05\x05"
    print("1) master-->write and slave-->read continuously")
    spi_obj.write_readinto(tx, rx)


spi_obj = spi_master_configure()

print("\n1: master-->write and slave-->read")
tx_buf = b"\x08\x06\x04\x02\x07\x05\x03\x01"
rx_buf = bytearray(8)
# spi_half_duplex_communication(spi_obj, tx_buf, rx_buf)
spi_obj.write(tx)
# _wait_for_slave_signal()
rx = spi_obj.read(8)


tx_buf = b"\x08\x06\x04\x02\x07\x05\x03\x01"
rx_buf = bytearray(8)
# spi_full_duplex_communication(spi_obj, tx_buf, rx_buf)

spi_obj.deinit()
spi_obj = soft_spi_master_configure()
spi_obj.deinit()
