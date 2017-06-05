import os
from machine import SPI, Pin
from sdcard import SDCard

def mount_sd():
    sd = SDCard(SPI(0), Pin("A22", mode=Pin.OUT))
    os.mount(sd, '/')

