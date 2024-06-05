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
    sck_slave_pin = "P6_2"
    mosi_slave_pin = "P6_0"
    miso_slave_pin = "P6_1"
    ssel_slave_pin = "P6_3"

if "CY8CPROTO-063-BLE" in machine:
    slave_write_notify_pin = "P9_5"  # Sends signals when master wants to write data
    slave_read_notify_pin = "P9_4"  # interrupt pin to check if slave is writing data
    # Allocate pin based on board
    sck_slave_pin = "P9_2"
    mosi_slave_pin = "P9_0"
    miso_slave_pin = "P9_1"
    ssel_slave_pin = "P9_3"

# signal_received = False


# def signal_irq(arg):
#     global signal_received
#     signal_received = True


# pin_out = Pin(slave_write_notify_pin, mode=Pin.OUT, value=True)
# pin_in = Pin(slave_read_notify_pin, Pin.IN)
# pin_in.irq(handler=signal_irq, trigger=Pin.IRQ_RISING)


# def _slave_ready_to_write():
#     pin_out.value(0)
#     time.sleep_ms(1000)
#     pin_out.value(1)


# def _wait_for_master_signal():
#     global signal_received
#     while not signal_received:
#         pass
#     signal_received = False


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
    return spi_obj


print("\n*** SPI SLAVE INSTANCE ***")

# spi_assert = lambda tx, rx: None if rx==tx else [print(f"tx: {tx}\nrx: {rx}")]
spi_assert = lambda tx, rx: [print(f"tx: {tx}\nrx: {rx}")]

spi_obj = spi_slave_configure()

# 1: master-->write and slave-->read
# tx_buf = b"\x08\x06\x04\x02\x07\x05\x03\x01"
rx_buf = bytearray(8)

# _wait_for_master_signal()
spi_obj.read(rx_buf)

for i in range(len(rx_buf)):
    rx_buf[i] += 1

# print(rx_buf)

# _slave_ready_to_write()
spi_obj.write(rx_buf)

spi_obj.deinit()
