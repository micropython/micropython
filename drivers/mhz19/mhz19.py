# MH-Z19 CO2 sensor driver for MicroPython.
# MIT license; Copyright (c) 2018 Fredrik Strupe

from machine import Pin
import utime


class TimeoutError(Exception):
    pass


class MHZ19:
    '''MH-Z19 CO2 sensor driver'''

    def __init__(self, pin, max_value=5000):
        '''
        Args:
            pin: the pin that the PWM pin on the MH-Z19 is connected to.
            max_value: upper bound of measuring range. usually 2000 or 5000.
        '''
        self.pin = pin
        self.max_value = max_value

    def _wait_on_condition(self, cond, timeout=5000):
        start = utime.ticks_ms()
        while not cond():
            if utime.ticks_diff(utime.ticks_ms(), start) > timeout:
                raise TimeoutError

    def _rising_callback(self, pin):
        self.cycle_started = True

    def pwm_read(self):
        '''Read CO2 value via PWM pin.

        Reading usually takes 1-2 seconds.

        Returns:
            CO2 value in ppm (parts per million), with an accuracy of
            ±(50 + result * 0.05) ppm.
        Raises:
            TimeoutError: if the reading takes more than 5 seconds.
        '''
        self.cycle_started = False

        # Wait until a new cycle starts
        self.pin.irq(trigger=Pin.IRQ_RISING, handler=self._rising_callback)
        self._wait_on_condition(lambda: self.cycle_started)
        self.pin.irq(trigger=Pin.IRQ_RISING, handler=None)

        # Measure pin high duration
        h_start = utime.ticks_ms()
        self._wait_on_condition(lambda: self.pin.value() == 0)

        # Measure pin low duration
        h_end = l_start = utime.ticks_ms()
        self._wait_on_condition(lambda: self.pin.value() == 1)
        l_end = utime.ticks_ms()

        # Calculate result
        t_h = utime.ticks_diff(h_end, h_start)
        t_l = utime.ticks_diff(l_end, l_start)
        return self.max_value * (t_h - 2) / (t_h + t_l - 4)
