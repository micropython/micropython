# Deneyap Mini v2 MicroPython Helper Library

from micropython import const
from machine import Pin


# Pin Assignments

# SPI
SPI_MOSI = const(40)
SPI_MISO = const(39)
SPI_CLK = const(38)

# I2C
I2C_SDA = const(36)
I2C_SCL = const(37)

# UART
UART_TX = const(43)
UART_RX = const(44)

# DAC
DAC0 = const(17)
DAC1 = const(18)

# LED
RGBLED = const(33)

# BUTTON
GPKEY = const(0)

# Built-in peripherals

rgbled = Pin(RGBLED, Pin.OUT, value=0)
gpkey = Pin(GPKEY, Pin.IN, Pin.PULL_UP)
