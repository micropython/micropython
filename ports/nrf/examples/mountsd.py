"""

Example for pca10040 / nrf52832 to show how mount
and list a sdcard connected over SPI.


Direct wiring on SD card (SPI):
   ______________________________
  |                              \
  |             9.    | NC    |   \
  |             1.      | ~CS   | |
  |             2.      | MOSI  | |
  |             3.      | GND   | |
  |             4.      | VCC3.3| |
  |             5.      | SCK   | |
  |             6.      | GND   | |
  |             7.      | MISO  | |
  |             8.      | NC    | |
  |                               |
  ---------------------------------
"""

import os, vfs
from machine import SPI, Pin
from sdcard import SDCard


def mnt():
    cs = Pin("P22", mode=Pin.OUT)
    sd = SDCard(SPI(0), cs)
    vfs.mount(sd, "/")


def list():
    files = os.listdir()
    print(files)
