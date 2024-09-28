# FeatherS3 Neo MicroPython Helper Library
# MIT license; Copyright (c) 2024 Seon Rozenblum - Unexpected Maker
#
# Project home:
# http://esp32s3.com

# Import required libraries
from micropython import const
from machine import Pin, ADC
import time

# FeatherS3 Hardware Pin Assignments

# Sense Pins
VBUS_SENSE = const(15)
VBAT_SENSE = const(2)

# LDO2 & Other Pins
LDO2 = const(39)
LED = const(13)
AMB_LIGHT = const(4)

# RGB LED Pins
RGB_DATA = const(40)
RGB_PWR = const(39)

# RGB MATRIX LED Pins
RGB_MATRIX_DATA = const(16)
RGB_MATRIX_PWR = const(39)

# SPI
SPI_MOSI = const(35)
SPI_MISO = const(37)
SPI_CLK = const(36)

# I2C
I2C_SDA = const(8)
I2C_SCL = const(9)

# Helper functions


# LED & Ambient Light Sensor control
def led_set(state):
    """Set the state of the BLUE LED on IO13"""
    l = Pin(LED, Pin.OUT)
    l.value(state)


def led_blink():
    """Toggle the BLUE LED on IO13"""
    l = Pin(LED, Pin.OUT)
    l.value(not l.value())


# Create ADC and set attenuation and return the ambient light value from the onboard sensor
def get_amb_light():
    """Get Ambient Light Sensor reading"""
    adc = ADC(Pin(AMB_LIGHT))
    adc.atten(ADC.ATTN_11DB)
    return adc.read()


def set_ldo2_power(state):
    """
    Enable or Disable power to the second LDO, which is the LDO that powers the following items
    RGB Matrix, RGB status LED, Ambient light Sensor.
    This is ON by default and will automatically shut down when the ESP32 going into deep sleep.
    """
    Pin(LDO2, Pin.OUT).value(state)


def get_battery_voltage():
    """
    Returns the current battery voltage. If no battery is connected, returns 4.2V which is the charge voltage
    This is an approximation only, but useful to detect if the charge state of the battery is getting low.
    """
    adc = ADC(Pin(VBAT_SENSE))  # Assign the ADC pin to read
    # Max voltage on ADC input will be 4.2V divided by R2 (442K) & R5 (160K), 4.2 / (160+442) * 160 = 1.1163V
    adc.atten(ADC.ATTN_2_5DB)  # Needs 2.5DB attenuation to read a max voltage of 1.1163V
    # Use read_uv() to get ADC reading as this will use the on-chip calibration data
    measuredvbat = adc.read_uv() / 1000000  # Read micovolts and convert to volts
    measuredvbat *= 3.7624  # Multiply by ratio of battery voltage to ADC pin voltage: 4.2/1.1163
    return round(measuredvbat, 2)


def get_vbus_present():
    """Detect if VBUS (5V) power source is present"""
    return Pin(VBUS_SENSE, Pin.IN).value() == 1


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
