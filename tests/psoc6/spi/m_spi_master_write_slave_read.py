import binascii
import time
import os

try:
    from machine import Pin, SPI
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

signal_received = False


def signal_irq(arg):
    global signal_received
    signal_received = True


pin_out = Pin(master_write_notify_pin, pull=Pin.PULL_UP, mode=Pin.OUT, value=True)
pin_in = Pin(master_read_notify_pin, pull=Pin.PULL_UP, mode=Pin.IN, value=True)
pin_in.irq(handler=signal_irq, trigger=Pin.IRQ_RISING)


def _master_ready_to_write():
    pin_out.value(0)  # high
    time.sleep_ms(1000)
    pin_out.value(1)  # low


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


# spi_assert = lambda tx, rx: None if rx==tx else [print(f"tx: {tx}\nrx: {rx}")]
spi_assert = lambda tx, rx: [print(f"\ntx: {tx}\nrx: {rx}")]

spi_obj = spi_master_configure()

print("master-->write and slave-->read")
tx_buf = b"\x08\x06\x04\x02\x07\x05\x03\x01"
rx_buf = bytearray(8)

_master_ready_to_write()
spi_obj.write(tx_buf)

_wait_for_slave_signal()
rx_buf = spi_obj.read(8)

spi_assert(tx_buf, rx_buf)

spi_obj.deinit()
