# LOLIN S2 MINI MicroPython Helper Library


from micropython import const
from machine import Pin, SPI, ADC
import machine, time

import network

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

#BUTTON
BUTTON = const(0)



class led:
    def __init__(self):
        self._led = Pin(LED, Pin.OUT, value=0)

    def on(self):
        self._led.value(1)
        
    def off(self):
        self._led.value(0)

class button:
    def __init__(self):
        self._btn = Pin(BUTTON, Pin.IN, Pin.PULL_UP)

    def get_button_status(self):
        return self._btn.value()

    def set_button_callback(self, cb):
        self._btn.irq(trigger=Pin.IRQ_FALLING, handler=cb)



