'''
SPI test for the CC3200 based boards.
'''

from machine import SPI
import os

mch = os.uname().machine
if 'LaunchPad' in mch:
    spi_pins = ('GP14', 'GP16', 'GP30')
elif 'WiPy' in mch:
    spi_pins = ('GP14', 'GP16', 'GP30')
else:
    raise Exception('Board not supported!')

spi = SPI(0, SPI.MASTER, baudrate=2000000, polarity=0, phase=0, firstbit=SPI.MSB, pins=spi_pins)
print(spi)
spi = SPI(baudrate=5000000)
print(spi)
spi = SPI(0, SPI.MASTER, baudrate=200000, bits=16, polarity=0, phase=0)
print(spi)
spi = SPI(0, SPI.MASTER, baudrate=10000000, polarity=0, phase=1)
print(spi)
spi = SPI(0, SPI.MASTER, baudrate=5000000, bits=32, polarity=1, phase=0)
print(spi)
spi = SPI(0, SPI.MASTER, baudrate=10000000, polarity=1, phase=1)
print(spi)
spi.init(baudrate=20000000, polarity=0, phase=0)
print(spi)
spi=SPI()
print(spi)
SPI(mode=SPI.MASTER)
SPI(mode=SPI.MASTER, pins=spi_pins)
SPI(id=0, mode=SPI.MASTER, polarity=0, phase=0, pins=('GP14', 'GP16', 'GP15'))
SPI(0, SPI.MASTER, polarity=0, phase=0, pins=('GP31', 'GP16', 'GP15'))

spi = SPI(0, SPI.MASTER, baudrate=10000000, polarity=0, phase=0, pins=spi_pins)
print(spi.write('123456') == 6)
buffer_r = bytearray(10)
print(spi.readinto(buffer_r) == 10)
print(spi.readinto(buffer_r, write=0x55) == 10)
read = spi.read(10)
print(len(read) == 10)
read = spi.read(10, write=0xFF)
print(len(read) == 10)
buffer_w = bytearray([1, 2, 3, 4, 5, 6, 7, 8, 9, 0])
print(spi.write_readinto(buffer_w, buffer_r) == 10)
print(buffer_w == buffer_r)

# test all polaritiy and phase combinations
spi.init(polarity=1, phase=0, pins=None)
buffer_r = bytearray(10)
spi.write_readinto(buffer_w, buffer_r)
print(buffer_w == buffer_r)

spi.init(polarity=1, phase=1, pins=None)
buffer_r = bytearray(10)
spi.write_readinto(buffer_w, buffer_r)
print(buffer_w == buffer_r)

spi.init(polarity=0, phase=1, pins=None)
buffer_r = bytearray(10)
spi.write_readinto(buffer_w, buffer_r)
print(buffer_w == buffer_r)

# test 16 and 32 bit transfers
buffer_w = bytearray([1, 2, 3, 4, 5, 6, 7, 8, 9, 0, 1, 2])
buffer_r = bytearray(12)
spi.init(SPI.MASTER, baudrate=10000000, bits=16, polarity=0, phase=0, pins=None)
print(spi.write_readinto(buffer_w, buffer_r) == 12)
print(buffer_w == buffer_r)

buffer_r = bytearray(12)
spi.init(SPI.MASTER, baudrate=10000000, bits=32, polarity=0, phase=0, pins=None)
print(spi.write_readinto(buffer_w, buffer_r) == 12)
print(buffer_w == buffer_r)

# check for memory leaks...
for i in range (0, 1000):
    spi = SPI(0, SPI.MASTER, baudrate=1000000)

# test deinit
spi = SPI(0, SPI.MASTER, baudrate=1000000)
spi.deinit()
print(spi)

spi = SPI(0, SPI.MASTER, baudrate=1000000)
# next ones must fail
try:
    spi = SPI(0, 10, baudrate=10000000, polarity=0, phase=0)
except:
    print("Exception")

try:
    spi = SPI(0, mode=SPI.MASTER, baudrate=10000000, polarity=1, phase=2)
except:
    print("Exception")

try:
    spi = SPI(1, mode=SPI.MASTER, baudrate=10000000, polarity=1, phase=1)
except:
    print("Exception")

try:
    spi = SPI(0, mode=SPI.MASTER, baudrate=2000000, polarity=2, phase=0)
except:
    print("Exception")

try:
    spi = SPI(0, mode=SPI.MASTER, baudrate=2000000, polarity=2, phase=0, firstbit=2)
except:
    print("Exception")

try:
    spi = SPI(0, mode=SPI.MASTER, baudrate=2000000, polarity=2, phase=0, pins=('GP1', 'GP2'))
except:
    print("Exception")

try:
    spi = SPI(0, mode=SPI.MASTER, baudrate=2000000, polarity=0, phase=0, bits=9)
except:
    print("Exception")

spi.deinit()
try:
    spi.read(15)
except Exception:
    print("Exception")

try:
    spi.spi.readinto(buffer_r)
except Exception:
    print("Exception")

try:
    spi.spi.write('abc')
except Exception:
    print("Exception")

try:
    spi.write_readinto(buffer_w, buffer_r)
except Exception:
    print("Exception")

# reinitialization must work
spi.init(baudrate=500000)
print(spi)
