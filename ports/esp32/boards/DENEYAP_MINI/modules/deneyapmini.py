# Deneyap Mini MicroPython Helper Library

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

# RGB LED
LEDR = const(34)
LEDG = const(33)
LEDB = const(35)

# BUTTON
GPKEY = const(0)

# Built-in peripherals

ledr = Pin(LEDR, Pin.OUT, value=0)
ledg = Pin(LEDG, Pin.OUT, value=0)
ledb = Pin(LEDB, Pin.OUT, value=0)
gpkey = Pin(GPKEY, Pin.IN, Pin.PULL_UP)
