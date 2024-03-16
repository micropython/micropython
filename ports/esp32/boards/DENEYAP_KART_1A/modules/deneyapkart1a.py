# Deneyap Kart 1A Helper Library

from micropython import const
from machine import Pin

# Pin Assignments

# SPI
SPI_MOSI = const(5)
SPI_MISO = const(18)
SPI_CLK = const(19)

# I2C
I2C_SDA = const(4)
I2C_SCL = const(15)

# DAC
DAC1 = const(25)
DAC2 = const(26)

# RGB LED
RGBLED = const(13)

# BUTTON
GPKEY = const(0)

# Built-in peripherals

rgbled = Pin(RGBLED, Pin.OUT, value=0)
gpkey = Pin(GPKEY, Pin.IN, Pin.PULL_UP)
