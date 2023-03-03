# GARATRONIC PYBSTICK26_C3 customisation

from micropython import const
from machine import Pin

# Pin Assignments

# SPI
SPI_MOSI = const(7)
SPI_MISO = const(2)
SPI_CLK = const(6)

# BUTTON
BUTTON = const(9)

# Built-in peripherals
button = Pin(BUTTON, Pin.IN, Pin.PULL_UP)

