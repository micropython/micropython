# show_pins.py
# show all pins of the nrf target board
# originally from @dhylands, "adapted" for nrf port by @rolandvs

import board
import machine

def pins():
    for pin_name in dir(machine.Pin.board):
        if not (pin_name == '__class__' or pin_name == '__name__'):
            pin = machine.Pin(str(pin_name))
            print('{:10s} {:s}'.format(pin_name, str(pin)))

pins()
