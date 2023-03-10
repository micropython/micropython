# SPDX-FileCopyrightText: 2018 Shawn Hymel for Adafruit Industries
#
# SPDX-License-Identifier: MIT

print("pins test")

"""
`adafruit_boardtest.boardtest_gpio`
====================================================
Toggles all available GPIO on a board. Verify their operation with an LED,
multimeter, another microcontroller, etc.

Run this script as its own main.py to individually run the test, or compile
with mpy-cross and call from separate test script.

* Author(s): Shawn Hymel for Adafruit Industries

Implementation Notes
--------------------

**Software and Dependencies:**

* Adafruit CircuitPython firmware for the supported boards:
  https://github.com/adafruit/circuitpython/releases

"""

import time

import board
import digitalio
import supervisor

__version__ = "0.0.0-auto.0"
__repo__ = "https://github.com/adafruit/Adafruit_CircuitPython_BoardTest.git"

# Constants
LED_ON_DELAY_TIME = 0.2  # Seconds
LED_OFF_DELAY_TIME = 0.2  # Seconds
LED_PIN_NAMES = ["L", "LED", "RED_LED", "GREEN_LED", "BLUE_LED"]

# Test result strings
PASS = "PASS"
FAIL = "FAIL"
NA = "N/A"


# Determine if given value is a number
def _is_number(val):
    try:
        float(val)
        return True
    except ValueError:
        return False


# Release pins
def _deinit_pins(gpios):
    for g in gpios:
        g.deinit()


# Toggle IO pins while waiting for answer
def _toggle_wait(pin_gpios):
    timestamp = time.monotonic()
    led_state = False
    failed = []
    for pg in pin_gpios:
        (pin, gpio) = pg
        print("Is pin %s toggling? [y/n]" % pin)
        done = False
        while not done:
            if led_state:
                if time.monotonic() > timestamp + LED_ON_DELAY_TIME:
                    led_state = False
                    timestamp = time.monotonic()
            else:
                if time.monotonic() > timestamp + LED_OFF_DELAY_TIME:
                    led_state = True
                    timestamp = time.monotonic()

            gpio.value = led_state
            if supervisor.runtime.serial_bytes_available:
                answer = input()
                if bool(answer == "y"):
                    done = True
                elif bool(answer == "n"):
                    failed += pin
                    done = True
    return failed


def buildPin(pin):
    gpio = digitalio.DigitalInOut(pin)
    return gpio


def run_test(pins):
    """
    Toggles all available GPIO on and off repeatedly.

    :param list[str] pins: list of pins to run the test on
    :return: tuple(str, list[str]): test result followed by list of pins tested
    """

    # Create a list of analog GPIO pins
    analog_pins = [p for p in pins if p[0] == "A" and _is_number(p[1])]

    # Create a list of digital GPIO
    digital_pins = [p for p in pins if p[0] == "D" and _is_number(p[1])]

    # Toggle LEDs if we find any
    gpio_pins = analog_pins + digital_pins

    if gpio_pins:
        # Print out the LEDs found
        print("GPIO pins found:", end=" ")
        for pin in gpio_pins:
            print(pin, end=" ")
        print("\n")

        # Create a list of IO objects for us to toggle
        gpios = [buildPin(getattr(board, p)) for p in gpio_pins]

        print("built GPIOs")
        # Set all IO to output
        for gpio in gpios:
            gpio.direction = digitalio.Direction.OUTPUT

        # Toggle pins while waiting for user to verify LEDs blinking
        result = _toggle_wait(zip(gpio_pins, gpios))

        # Release pins
        _deinit_pins(gpios)

        if result:
            return FAIL, gpio_pins

        return PASS, gpio_pins

    # Else (no pins found)
    print("No GPIO pins found")
    return NA, []


run_test([p for p in dir(board)])
