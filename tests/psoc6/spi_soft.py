#### SoftSPI
import os
from machine import SoftSPI

# Allocate pin based on board
machine = os.uname().machine
if "CY8CPROTO-062-4343W" in machine:
    sck_pin = "P9_2"
    mosi_pin = "P9_0"
    miso_pin = "P9_1"
elif "CY8CPROTO-063-BLE" in machine:
    sck_pin = "P9_2"
    mosi_pin = "P9_0"
    miso_pin = "P9_1"

spi = SoftSPI(baudrate=100000, polarity=1, phase=0, sck=sck_pin, mosi=mosi_pin, miso=miso_pin)
print(spi)
spi.init(baudrate=200000)
print(spi)
