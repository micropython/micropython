# LOLIN S2 PICO MicroPython Helper Library

from micropython import const
from machine import Pin, I2C, Signal
from s2pico_oled import OLED

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

# LED
LED = const(10)

# OLED
OLED_RST = const(18)

# BUTTON
BUTTON = const(0)

# Helper methods for built in sensors

led = Signal(LED, Pin.OUT, value=0, invert=True)

button = Pin(BUTTON, Pin.IN, Pin.PULL_UP)

i2c = I2C(0)
oled = OLED(i2c, Pin(OLED_RST))
