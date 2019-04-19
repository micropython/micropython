# MicroPython PWM hardware test.
# This test uses a loopback from a timer pin to a non timer pin.
# It verifies the frequency and duty cycle of every timer on every pin.

# (C) 2019 Chris Mason
# Licenced under the terms of the MicroPython MIT licence.
# https://github.com/micropython/micropython/blob/master/LICENSE

import pyb
import machine
import sys
from pyb import Timer
from pyb import Pin
from pyb import LED
import micropython
micropython.alloc_emergency_exception_buf(100)

# ----------------------------------------------------------
PROBE_PIN = 'G0'  # This pin listens to each PWM pin
PWM_PERIOD = 100  # mSec
PWM_DUTY_CYCLE = 70  # %
PWM_TOLERANCE = 1    # Allowable measurement error in mSec.
# Don't use LEDs if they clash with PWM tests on same pins.
USE_LEDS = False

available_timers = (1, 2, 3, 4, 5, 8, 9, 10, 11, 12, 13, 14)

available_pwm_pins = (
    ('A0', 2, 1),
    ('A0', 5, 1),
    ('A1', 2, 2),
    ('A1', 5, 2),
    ('A2', 2, 3),
    ('A2', 5, 3),
    ('A2', 9, 1),
    ('A3', 2, 4),
    ('A3', 5, 4),
    ('A3', 9, 2),
    ('A5', 2, 1),
    ('A5', 8, 1, 'N'),
    ('A6', 3, 1),
    ('A6', 13, 1),
    ('A7', 1, 1, 'N'),
    ('A7', 3, 2),
    ('A7', 8, 1, 'N'),
    ('A7', 14, 1),

    ('A8', 1, 1),
# USB    ('A9', 1, 2),
# USB    ('A10', 1, 3),
# USB    ('A11', 1, 4),
    ('A15', 2, 1),

    ('B0', 1, 2, 'N'),
    ('B0', 3, 3),
    ('B0', 8, 2, 'N'),
    ('B1', 1, 3, 'N'),
    ('B1', 3, 4),
    ('B1', 8, 3, 'N'),
    ('B3', 2, 2),
    ('B4', 3, 1),
    ('B5', 3, 3),
    ('B6', 4, 1),
    ('B7', 4, 2),

    ('B8', 4, 3),
    ('B8', 10, 1),
    ('B9', 4, 4),
    ('B9', 11, 1),
    ('B10', 2, 3),
    ('B11', 2, 4),
    ('B13', 1, 1, 'N'),
    ('B14', 1, 2, 'N'),
    ('B14', 8, 2, 'N'),
    ('B14', 12, 1),
    ('B15', 1, 3, 'N'),
    ('B15', 8, 3, 'N'),
    ('B15', 12, 2),

    ('C6', 3, 1),
    ('C6', 8, 1),
    ('C7', 3, 2),
    ('C7', 8, 2),
    ('C8', 3, 3),
    ('C8', 8, 3),
    ('C9', 3, 4),
    ('C9', 8, 4),
   
    ('D12', 4, 1),
    ('D12', 4, 2),
    ('D12', 4, 3),
    ('D12', 4, 4),

    ('E5', 9, 1),
    ('E6', 9, 2),
    ('E8', 1, 1, 'N'),
    ('E9', 1, 1),
    ('E10', 1, 2, 'N'),
    ('E11', 1, 2),
    ('E12', 1, 3, 'N'),
    ('E13', 1, 3),
    ('E14', 1, 4),

    ('F3', 5, 1),
    ('F4', 5, 2),
    ('F5', 5, 3),
    ('F6', 10, 1),
    ('F7', 11, 1),
    ('F8', 13, 1),
    ('F9', 14, 1),
    ('F10', 5, 4),

)
# ----------------------------------------------------------
try:
    if (USE_LEDS):
        red_led = LED(1) # Indicates off time
        green_led = LED(2)
        blue_led = LED(3)  # Indicates on time.

    edge_count = 0
    low_time = 0
    high_time = 0

    pwm_pin = pyb.Pin(available_pwm_pins[0][0], Pin.OUT_PP)

    old_time = pyb.millis()

    def pwm_callback(x):
        global old_time
        global edge_count
        global high_time
        global low_time
        new_time = pyb.millis()
        if(edge_count < 5):
            pin_val = pwm_pin.value()
            if (USE_LEDS):
                if(pin_val == 1):
                    blue_led.on()
                    red_led.off()
                else:
                    blue_led.off()
                    red_led.on()
            if(edge_count > 2):
                if(pin_val == 0): # It was high before interrupt.
                    high_time = new_time - old_time
                else:           # It was low before interrupt.
                    low_time = new_time - old_time
            old_time = new_time
            if (edge_count != -1):
                edge_count += 1

    probing_pin = pyb.ExtInt(pyb.Pin(PROBE_PIN), pyb.ExtInt.IRQ_RISING_FALLING, pyb.Pin.PULL_UP, pwm_callback)
    
    # Before setting up a pin for test it may be high impedance and generate noise and false interrupts.
    # Instantiating a pin may generate and edge. Keep edge_count at -1 until stable.
    # The first on time is inacurate becuase there is an uncertain time between resetting the timer
    #    and setting the PWM particularly if a system interrupt occurs.
    #    The IRQ hander ingnores the first cycle then measures the second cycle.

    print('When a pin name appears connect the probe pin P' + PROBE_PIN + ' to it and press Enter\n')
    print('To test a different pin type the pin name, connect the probe pin P' + PROBE_PIN + ' to it and press Enter\n')

    last_pin = ''
    i = 0
    while (i < len(available_pwm_pins)):
        next_pin = available_pwm_pins[i]
        if(next_pin[0] != last_pin):
            inp = input('P' + next_pin[0] + '   ')
            if (inp != ''):
                for j in range(0, len(available_pwm_pins)):
                    if inp == ('P' + available_pwm_pins[j][0]):
                        print('')
                        print(inp)
                        i = j # Jump to the requested pin.
                        break
                if (j == len(available_pwm_pins) - 1):
                    print('Pin', inp, 'not found \n')
                    print('P' + next_pin[0])
        next_pin = available_pwm_pins[i]
        if ((len(next_pin) == 4) and (next_pin[3] == 'N')):
            pol = 'N'
        else:
            pol = ''
        low_time = 0
        high_time = 0
        edge_count = -1 # Delay measurement.
        pwm_pin = pyb.Pin(next_pin[0], Pin.OUT_PP)
        pwm_pin.value(0)
        pwm_timer = Timer(next_pin[1], freq = 1000/PWM_PERIOD)
        pwm_timer.counter(0)
        pwm_pin_PWM = pwm_timer.channel(next_pin[2], pyb.Timer.PWM, pin=pwm_pin, pulse_width_percent=PWM_DUTY_CYCLE)
        pyb.delay(10) # Wait for stable output.
        edge_count = 0 # Enable measurement cycle.

        next_pin_start = pyb.millis()
        while((edge_count < 5) and (pyb.millis() < (next_pin_start + PWM_PERIOD * 4))):
            pass
        if (pol == 'N'):
            on_time = low_time
            off_time = high_time
        else:
            on_time = high_time
            off_time = low_time
        if(
            (on_time >= (PWM_PERIOD * PWM_DUTY_CYCLE / 100 - PWM_TOLERANCE)) and
            (on_time <= (PWM_PERIOD * PWM_DUTY_CYCLE / 100 + PWM_TOLERANCE)) and
            (off_time >= (PWM_PERIOD * (100 - PWM_DUTY_CYCLE) / 100 - PWM_TOLERANCE)) and
            (off_time <= (PWM_PERIOD * (100 - PWM_DUTY_CYCLE) / 100 + PWM_TOLERANCE))
        ):
            result = 'pass'
        else:
            result = 'fail'
        print(on_time, off_time, 'tim' + str(next_pin[1]), 'ch' + str(next_pin[2]) + pol, result)
        last_pin = next_pin[0]
        pwm_timer.freq(4000/PWM_PERIOD)  # Set frequency and duty cycle different to next test.
        pwm_pin_PWM = pwm_timer.channel(next_pin[2], pyb.Timer.PWM, pin=pwm_pin, pulse_width_percent=0)
        i += 1
finally:
    probing_pin = pyb.ExtInt(pyb.Pin(PROBE_PIN), pyb.ExtInt.IRQ_RISING_FALLING, pyb.Pin.PULL_UP, None)
    print('\nFinished')
