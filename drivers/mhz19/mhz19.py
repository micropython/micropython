# MH-Z19 CO2 sensor driver for MicroPython.
# MIT license; Copyright (c) 2018 Fredrik Strupe

import machine
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

    def pwm_read(self):
        '''Read CO2 value via PWM pin.

        Reading usually takes 1-2 seconds.

        Returns:
            CO2 value in ppm (parts per million), with an accuracy of
            ±(50 + result * 0.05) ppm.
        Raises:
            TimeoutError: if the reading takes more than 5 seconds.
        '''
        # Wait until a new cycle starts
        self._wait_on_condition(lambda: self.pin.value() == 0)
        self._wait_on_condition(lambda: self.pin.value() == 1)

        # Measure high and low duration during cycle
        t_h = machine.time_pulse_us(self.pin, 1, 1500000) / 1000
        t_l = machine.time_pulse_us(self.pin, 0, 1500000) / 1000

        return self.max_value * (t_h - 2) / (t_h + t_l - 4)
