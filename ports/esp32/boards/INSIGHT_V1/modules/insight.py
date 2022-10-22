# INSIGHT MicroPython Helper Library
# 2022 Patrick Joy - Think Transit
#
#
#
# 2022-Apr-22 - v0.1 - Initial version

# Import required libraries
from micropython import const
from machine import Pin, SPI, ADC
import machine, time



# SPI
SPI_MOSI = const(35)
SPI_MISO = const(37)
SPI_CLK = const(36)

# I2C
I2C_SDA = const(8)
I2C_SCL = const(9)


# Go into deep sleep but shut down the APA first to save power
# Use this if you want lowest deep  sleep current
def go_deepsleep(t):
    """Deep sleep helper that also powers down the on-board Dotstar."""
    machine.deepsleep(t)
