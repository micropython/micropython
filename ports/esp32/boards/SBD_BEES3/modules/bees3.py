# Import required libraries
from micropython import const
from machine import Pin, ADC
import time


VBAT_SENSE = const(1)

# RGB LED
RGB_DATA = const(48)
RGB_PWR = const(34)

# SPI
SPI_MOSI = const(35)
SPI_MISO = const(38)
SPI_CLK = const(39)

# I2C
I2C_SDA = const(37)
I2C_SCL = const(36)



# Helper functions

def set_rgb_power(state):
    """Enable or Disable power to the second LDO"""
    Pin(RGB_PWR, Pin.OUT).value(state)


def get_battery_voltage():
    """
    Returns the current battery voltage.
    """
    adc = ADC(Pin(VBAT_SENSE))  # Assign the ADC pin to read
    # We are going to read the ADC 10 times and discard the results as we can't guarantee the ADC is calibrated or stable yet after boot. Not sure why we have to do this :(
    for _ in range(10):
        adc.read()
    measuredvbat = adc.read()
    measuredvbat /= 4095  # divide by 4095 as we are using the default ADC voltage range of 0-1.2V
    measuredvbat *= 4.2  # Multiply by 4.2V, our max charge voltage for a 1S LiPo
    return round(measuredvbat, 2)



# NeoPixel rainbow colour wheel
def rgb_color_wheel(wheel_pos):
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

