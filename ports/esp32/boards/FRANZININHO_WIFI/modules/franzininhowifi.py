# Franzininho WiFi MicroPython Helper Library

from micropython import const
from machine import Pin

# Pin Assignments

# SPI
SPI_MOSI = const(35)
SPI_MISO = const(36)
SPI_CLK = const(37)

# I2C
I2C_SDA = const(8)
I2C_SCL = const(9)

# DAC
DAC1 = const(17)
DAC2 = const(18)

# LEDs
LED_YELLOW = const(33)
LED_BLUE = const(21)

# BUTTON
BUTTON = const(0)

# Helper methods for built in sensors

led_yellow = Pin(LED_YELLOW, Pin.OUT, value=0)
led_blue = Pin(LED_BLUE, Pin.OUT, value=0)

button = Pin(BUTTON, Pin.IN, Pin.PULL_UP)
