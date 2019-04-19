# MicroPython ADC & DAC hardware test.
# These tests verify the DACs using a voltmeter.
# They verify the ADC channels using loopback from a DAC to ADC pin.

# (C) 2019 Chris Mason
# Licenced under the terms of the MicroPython MIT licence.
# https://github.com/micropython/micropython/blob/master/LICENSE

import pyb
import machine
import sys
from pyb import ADC
from pyb import DAC
from pyb import Pin
from pyb import LED
import micropython
micropython.alloc_emergency_exception_buf(100)

# ----------------------------------------------------------
ADC_TOLERANCE = 1    # Allowable measurement error in raw 12 bit value.

available_DACs = (1, 2)

available_ADC_pins = (
    'PA0',
    'PA1',
    'PA2',
    'PA3',
    'PA4',
    'PA5',
    'PA6',
    'PA7',
    'PB0',
    'PB1',
    'PC0',
    'PC1',
    'PC2',
    'PC3',
    'PC4',
    'PC5',

)
# ----------------------------------------------------------
try:
    for i in available_DACs:
        dac = DAC(i, bits=12)
        dac.write(int(4095*(1/3.3)))
        inp = input('DAC' + str(i) + ' should be 1V. Press Enter to continue.')
        dac.write(int(4095*(2/3.3)))
        inp = input('DAC' + str(i) + ' should be 2V. Press Enter to continue.')

    dac = DAC(1, bits=12)
    for next_pin in available_ADC_pins:
        adc = ADC(next_pin)
        dac.write(int(4095*(1/3.3)))
        pyb.delay(1)
        inp = input('Connect DAC1 to ' + next_pin + ' and press Enter. ADC should be 1V, then 2V.')
        val1 = adc.read()
        dac.write(int(4095*(2/3.3)))
        pyb.delay(1)
        val2 = adc.read()
        print(next_pin, val1*3.3/4095, val2*3.3/4095, '\n')

finally:
    print('\noops')
