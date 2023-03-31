# Deneyap Kart 1A v2 Helper Library

from micropython import const
from machine import Pin

# Pin Assignments

# SPI
SPI_MOSI = const(39)
SPI_MISO = const(40)
SPI_CLK = const(41)

# I2C
I2C_SDA = const(47)
I2C_SCL = const(21)

# UART
UART_TX = const(43)
UART_RX = const(44)

# RGB LED
RGBLED = const(48)

# BUTTON
GPKEY = const(0)

# Built-in peripherals

rgbled = Pin(RGBLED, Pin.OUT, value=0)
gpkey = Pin(GPKEY, Pin.IN, Pin.PULL_UP)
