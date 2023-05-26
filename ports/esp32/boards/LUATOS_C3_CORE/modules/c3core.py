# LUAT C3 CORE MicroPython Helper Library

from micropython import const
from machine import Pin

# Pin Assignments

# SPI
SPI_MOSI = const(3)
SPI_MISO = const(10)
SPI_SCLK = const(2)

# I2C
I2C_SDA = const(4)
I2C_SCL = const(5)

# LED
LED = const(12)
LED1 = const(12)
LED2 = const(13)

# BUTTON
BUTTON = const(9)

# Built-in peripherals

# leds
led1 = Pin(LED1, Pin.OUT, value=0)
led2 = Pin(LED2, Pin.OUT, value=0)
led = led1

# buttons
button = Pin(BUTTON, Pin.IN, Pin.PULL_UP)
