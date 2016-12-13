import os
from machine import SPI
from pyb import Pin
from sdcard import SDCard

def mount_sd():
    sd = SDCard(SPI(0), Pin("A22"))
    os.mount(sd, '/')

