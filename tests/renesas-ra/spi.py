import os
from machine import SPI

machine = os.uname().machine
if (
    "RA6M1_EK" in machine
    or "RA4M1_CLICKER" in machine
    or "RA4M1_EK" in machine
    or "RA4W1_EK" in machine
    or "RA6M1_EK" in machine
):
    spis = (-1, 0)
else:
    spis = (-1, 0, 1)

# test we can correctly create by id
for bus in spis:
    try:
        spi = SPI(bus)
    except ValueError:
        print("ValueError", bus)

spi = SPI(0)
print(spi)

spi = SPI(0)
spi = SPI(0, baudrate=500000)
print(spi)
spi = SPI(0, 500000, polarity=1, phase=0, bits=8)
print(spi)
spi.init(baudrate=400000)
print(spi)
spi.init(polarity=0)
print(spi)
spi.init(phase=1)
print(spi)
spi.init(bits=8)
print(spi)

spi.deinit()
