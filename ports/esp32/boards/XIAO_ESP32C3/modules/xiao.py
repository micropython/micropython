# XIAO ESP32 C3 MicroPython Helper Library

from micropython import const
from machine import Pin

# Pin Assignments

# SPI
SPI_MOSI = const(10)
SPI_MISO = const(9)
SPI_CLK = const(8)

# I2C
I2C_SDA = const(6)
I2C_SCL = const(7)
