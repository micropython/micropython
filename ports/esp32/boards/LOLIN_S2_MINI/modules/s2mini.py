# LOLIN S2 MINI MicroPython Helper Library

from micropython import const
from machine import Pin

# Pin Assignments

# SPI
SPI_MOSI = const(11)
SPI_MISO = const(9)
SPI_CLK = const(7)

# I2C
I2C_SDA = const(33)
I2C_SCL = const(35)

# DAC
DAC1 = const(17)
DAC2 = const(18)

# LED
LED = const(15)

# BUTTON
BUTTON = const(0)

# Helper methods for built in sensors

led = Pin(LED, Pin.OUT, value=0)

button = Pin(BUTTON, Pin.IN, Pin.PULL_UP)
