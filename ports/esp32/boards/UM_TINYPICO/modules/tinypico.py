# TinyPICO MicroPython Helper Library
# 2019 Seon Rozenblum, Matt Trentini
#
# Project home:
#   https://github.com/TinyPICO
#
# 2019-Mar-12 - v0.1 - Initial implementation
# 2019-May-20 - v1.0 - Initial Release
# 2019-Oct-23 - v1.1 - Removed temp sensor code, prep for frozen modules

# Import required libraries
from micropython import const
from machine import Pin, SPI, ADC
import machine, time, esp32

# TinyPICO Hardware Pin Assignments

# Battery
BAT_VOLTAGE = const(35)
BAT_CHARGE = const(34)

# APA102 Dotstar pins for production boards
DOTSTAR_CLK = const(12)
DOTSTAR_DATA = const(2)
DOTSTAR_PWR = const(13)

# SPI
SPI_MOSI = const(23)
SPI_CLK = const(18)
SPI_MISO = const(19)

# I2C
I2C_SDA = const(21)
I2C_SCL = const(22)

# DAC
DAC1 = const(25)
DAC2 = const(26)

# Helper functions

# Get a *rough* estimate of the current battery voltage
# If the battery is not present, the charge IC will still report it's trying to charge at X voltage
# so it will still show a voltage.
def get_battery_voltage():
    """
    Returns the current battery voltage. If no battery is connected, returns 3.7V
    This is an approximation only, but useful to detect of the charge state of the battery is getting low.
    """
    adc = ADC(Pin(BAT_VOLTAGE))  # Assign the ADC pin to read
    measuredvbat = adc.read()  # Read the value
    measuredvbat /= 4095  # divide by 4095 as we are using the default ADC voltage range of 0-1V
    measuredvbat *= 3.7  # Multiply by 3.7V, our reference voltage
    return measuredvbat


# Return the current charge state of the battery - we need to read the value multiple times
# to eliminate false negatives due to the charge IC not knowing the difference between no battery
# and a full battery not charging - This is why the charge LED flashes
def get_battery_charging():
    """
    Returns the current battery charging state.
    This can trigger false positives as the charge IC can't tell the difference between a full battery or no battery connected.
    """
    measuredVal = 0  # start our reading at 0
    io = Pin(BAT_CHARGE, Pin.IN)  # Assign the pin to read

    for y in range(
        0, 10
    ):  # loop through 10 times adding the read values together to ensure no false positives
        measuredVal += io.value()

    return measuredVal == 0  # return True if the value is 0


# Power to the on-board Dotstar is controlled by a PNP transistor, so low is ON and high is OFF
# We also need to set the Dotstar clock and data pins to be inputs to prevent power leakage when power is off
# This might be improved at a future date
# The reason we have power control for the Dotstar is that it has a quiescent current of around 1mA, so we
# need to be able to cut power to it to minimise power consumption during deep sleep or with general battery powered use
# to minimise unneeded battery drain
def set_dotstar_power(state):
    """Set the power for the on-board Dotstar to allow no current draw when not needed."""
    # Set the power pin to the inverse of state
    if state:
        Pin(DOTSTAR_PWR, Pin.OUT, None, value=0)  # Drive output to LOW to enable transistor
    else:
        Pin(DOTSTAR_PWR, Pin.IN, None)  # Disable output, external pull-up will disable transistor

    Pin(
        DOTSTAR_CLK, Pin.OUT if state else Pin.IN
    )  # If power is on, set CLK to be output, otherwise input
    Pin(
        DOTSTAR_DATA, Pin.OUT if state else Pin.IN
    )  # If power is on, set DATA to be output, otherwise input

    # A small delay to let the IO change state
    time.sleep(0.035)


# Dotstar rainbow colour wheel
def dotstar_color_wheel(wheel_pos):
    """Color wheel to allow for cycling through the rainbow of RGB colors."""
    wheel_pos = wheel_pos % 255

    if wheel_pos < 85:
        return 255 - wheel_pos * 3, 0, wheel_pos * 3
    elif wheel_pos < 170:
        wheel_pos -= 85
        return 0, wheel_pos * 3, 255 - wheel_pos * 3
    else:
        wheel_pos -= 170
        return wheel_pos * 3, 255 - wheel_pos * 3, 0


# Go into deep sleep but shut down the APA first to save power
# Use this if you want lowest deep  sleep current
def go_deepsleep(t):
    """Deep sleep helper that also powers down the on-board Dotstar."""
    set_dotstar_power(False)
    machine.deepsleep(t)
