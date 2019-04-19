# MicroPython on board LED test.

# (C) 2019 Chris Mason
# Based on the MicroPython LED tutorial.
# Licenced under the terms of the MicroPython MIT licence.
# https://github.com/micropython/micropython/blob/master/LICENSE

import pyb
available_leds = [1,2,3]
leds = [pyb.LED(i) for i in available_leds]
for led in leds:
    led.off()

n = 0
try:
    while True:
        for led in leds:
            led.on()
            pyb.delay(200)
            led.off()
finally:
    for led in leds:
        led.off()
