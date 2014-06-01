"""
switch.py
=========

Light up all the leds when the USR switch on the pyboard is pressed.
"""

import pyb

switch = pyb.Switch()
red_led = pyb.LED(1)
green_led = pyb.LED(2)
orange_led = pyb.LED(3)
blue_led = pyb.LED(4)
leds = [red_led, green_led, orange_led, blue_led]

while 1:
    if switch():
        ## red_led.on()
        ## green_led.on()
        ## orange_led.on()
        ## blue_led.on()
        [led.on() for led in leds]
    else:
        ## red_led.off()
        ## green_led.off()
        ## orange_led.off()
        ## blue_led.off()
        [led.off() for led in leds]
