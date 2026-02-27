# XIAO ESP32 S3 MicroPython Helper Library

from micropython import const
from machine import Pin

# SPI
SPI_MOSI = const(9)
SPI_MISO = const(8)
SPI_CLK = const(7)

# I2C
I2C_SDA = const(5)
I2C_SCL = const(6)

# LED
LED = const(21)

# Helper methods for built in sensors

led = Pin(LED, Pin.OUT, value=0)
