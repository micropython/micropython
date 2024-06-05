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
    # Allocate pin based on board
    sck_master_pin = "P6_2"
    mosi_master_pin = "P6_0"
    miso_master_pin = "P6_1"
    ssel_master_pin = "P6_3"
if "CY8CPROTO-063-BLE" in machine:
    # Allocate pin based on board
    sck_master_pin = "P9_2"
    mosi_master_pin = "P9_0"
    miso_master_pin = "P9_1"
    ssel_master_pin = "P9_3"

# try:
#     spi = SPI(ssel=ssel_master_pin)
# except Exception as e:
#     print(e)

# 0. Construct SPI object

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

# 1. write() read() validation
# - Master: write() a data set to the slave.
# - Slave: waits and read() data set.
# - Slave: increase all data set bytes by 1.
# - Slave: write() the modified data to the master.

tx_buf = b"\x08\x06\x04\x02\x07\x05\x03\x01"

spi_obj.write(tx_buf)

time.sleep_ms(500)  # ensure slave has replied
rx_buf = bytearray(8)
rx_buf = spi_obj.read(8)

exp_rx = b"\t\x07\x05\x03\x08\x06\x04\x02"
print("master write() and read() (tx_buf + 1): ", exp_rx == rx_buf)

# 2. readinto() validation
# - Slave: increase once again all data set bytes by 1.
# - Slave: write() the modified data set to the master.
# - Master: waits and readinto() the modified data set.

time.sleep_ms(500)  # ensure slave has replied
rx_buf = bytearray(8)
spi_obj.readinto(rx_buf)

exp_rx = b"\n\x08\x06\x04\t\x07\x05\x03"
print("master readinto() (tx_buf + 2):", exp_rx == rx_buf)

# 3. write_readinto() read validation
# - Slave: increase once again all data set bytes by 1.
# - Slave: write() the modified data set to the master.
# - Master: waits and readinto() the data set (written data set is not processed).

time.sleep_ms(200)
rx_buf = bytearray(8)
spi_obj.write_readinto(tx_buf, rx_buf)

exp_rx = b"\x0b\t\x07\x05\n\x08\x06\x04"
print("master write_readinto() (tx_buf + 3): ", exp_rx == rx_buf)

# 4. write_readinto() write validation
# - Master: write_readinto() a data set to the slave (read data is ignored).
# - Slave: waits and read() data set.
# - Slave: decreases all data set bytes by 1.
# - Slave: write() the modified data set to the master.
# - Master: waits and read() the modified data set.

tx_buf = b"\x01\x02\x03\x04\x05\x06\x07\x08"
spi_obj.write_readinto(tx_buf, rx_buf)

time.sleep_ms(500)  # ensure slave has replied
rx_buf = bytearray(8)
rx_buf = spi_obj.read(8)

exp_rx = b"\x00\x01\x02\x03\x04\x05\x06\x07"
print("master write_readinto() and read() (tx_buf - 1): ", exp_rx == rx_buf)

# 5. SPI object deinit
spi_obj.deinit()
