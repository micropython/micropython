# Deneyap Kart G MicroPython Helper Library

from micropython import const
from machine import Pin

# Pin Assignments

# SPI
SPI_MOSI = const(6)
SPI_MISO = const(5)
SPI_CLK = const(4)

# I2C
I2C_SDA = const(8)
I2C_SCL = const(2)

# UART
UART_TX = const(21)
UART_RX = const(20)

# RGB LED
RGBLED = const(10)

# BUTTON
GPKEY = const(9)

# Built-in peripherals

rgbled = Pin(RGBLED, Pin.OUT, value=0)
gpkey = Pin(GPKEY, Pin.IN, Pin.PULL_UP)
