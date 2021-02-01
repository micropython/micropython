"""
switch.py
=========

Light up some leds when the USR switch on the pyboard is pressed.

Example Usage::

    Micro Python v1.0.1 on 2014-05-12; PYBv1.0 with STM32F405RG
    Type "help()" for more information.
    >>> import switch
    >>> switch.run_loop()
    Loop started.
    Press Ctrl+C to break out of the loop.

"""

import pyb

switch = pyb.Switch()
red_led = pyb.LED(1)
green_led = pyb.LED(2)
orange_led = pyb.LED(3)
blue_led = pyb.LED(4)
all_leds = (red_led, green_led, orange_led, blue_led)


def run_loop(leds=all_leds):
    """
    Start the loop.

    :param `leds`: Which LEDs to light up upon switch press.
    :type `leds`: sequence of LED objects
    """
    print("Loop started.\nPress Ctrl+C to break out of the loop.")
    while 1:
        try:
            if switch():
                [led.on() for led in leds]
            else:
                [led.off() for led in leds]
        except OSError:  # VCPInterrupt # Ctrl+C in interpreter mode.
            break


if __name__ == "__main__":
    run_loop()
