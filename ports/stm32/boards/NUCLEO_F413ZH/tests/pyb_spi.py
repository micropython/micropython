# MicroPython SPI hardware test.
# Set which SPI ports to test in "available_SPIs".
# Set the corresponding CS pins in "available_CSs".
# Connect each SPI port to a deterministic readable SPI device.
# This program expects a 25AA02 EEPROM with known data,
#  eg 25AA02E48T which has a pre-programmed MAC address.
# A single SPI device can be moved from port to port.
# The first 6 bytes of memory will be displayed.

# (C) 2019 Chris Mason
# Licenced under the terms of the MicroPython MIT licence.
# https://github.com/micropython/micropython/blob/master/LICENSE

import pyb
from pyb import Pin
from pyb import SPI
import sys
import ubinascii

# -----------------------------------------------
expected_data = b'00,04,a3,8e,4a,8f'

available_SPIs = [1, 2, 3, 4, 5]
available_CSs  = [pyb.Pin.cpu.A4, pyb.Pin.cpu.B12, pyb.Pin.cpu.A15, pyb.Pin.cpu.E4, pyb.Pin.cpu.E11]
# -----------------------------------------------

if (len(available_SPIs) != len(available_CSs)):
    print('length of available_SPIs and available_CSs do not match')
    sys.exit()
spis = []
css = []

for i in range(0, max(available_SPIs)):
    spis.append('none')
    css.append('none')
    if (i+1) in available_SPIs:
        spis[i] = SPI(i+1, SPI.MASTER,  baudrate=1000000, polarity=0, phase=0, bits=8, firstbit=SPI.MSB)
        css[i] = pyb.Pin(available_CSs[i], Pin.OUT_PP)
        css[i].value(1)

try:
    while True:
        for i in available_SPIs:
            print('SPI' + str(i))
            css[i-1].value(0)
            pyb.udelay(10)
            spis[i-1].send(3)         # EEPROM READ command
            spis[i-1].send(0xFA)         # from address of MAC address
            res = spis[i-1].recv(6, timeout=5000)    # read 6 bytes
            res = ubinascii.hexlify(res, ',')
            print(res)
            if (res == expected_data):
                print('passed')
            else:
                print('failed, expected: ', expected_data)
            css[i-1].value(1)
            pyb.udelay(10)
            pyb.delay(100)
        print('------------------')
finally:
    print('FAIL, SPI ', i, '\n')