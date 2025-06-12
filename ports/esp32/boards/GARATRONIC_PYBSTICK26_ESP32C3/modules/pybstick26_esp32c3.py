# PYBSTICK26-ESP32C3 MicroPython Helper Library
# V1.0 by fpie for GARATRONIC (2025)
#

from micropython import const
from machine import Pin

# Pin Assignments

# SPI
SPI_MOSI = const(7)
SPI_MISO = const(2)
SPI_CLK = const(6)

# I2C
I2C_SDA = const(0)
I2C_SCL = const(1)

# LED
LED = const(8)

# BUTTONS
BUTTON_A = const(9)
BUTTON_B = const(5)

# Built-in peripherals

led = Pin(LED, Pin.OUT, value=0)
button = Pin(BUTTON_B, Pin.IN, Pin.PULL_UP)
