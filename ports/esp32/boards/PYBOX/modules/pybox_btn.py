"""
    Capacitive Touch Buttons with Press and Long-Press detection
    Author: Wolf Paulus wolf@paulus.com
    MIT license; Copyright (c) 2022 wolfpaulus.com
"""
from machine import Pin, TouchPad
from utime import ticks_ms, ticks_diff
import pybox_ct as ct


class Button(TouchPad):
    """Capacitive Touch Button"""

    def __init__(
        self,
        pin: int,
        on_press: callable = None,
        on_long_press: callable = None,
        threshold: int = 600,
        hold_ms: int = 500,
        freq_ms: int = 100,
    ):
        """
        :param pin: port pin this button is connected to
        :param on_press: function to be called on touch
        :param on_long_press: function to be called on long touch
        :param threshold: capacitive touch value that qualifies as a touch
        :param hold_ms: number of ms button needs to be touch to register a long touch
        :param freq_ms: number of ms between checks
        """
        super().__init__(Pin(pin))
        self.on_press = on_press
        self.on_long_press = on_long_press
        self._threshold = threshold
        self._hold = hold_ms
        self._freq = freq_ms
        self._acted = False
        self._started = 0
        self._active = True

    def set_actions(self, on_press: callable = None, on_long_press: callable = None):
        self.on_press = on_press
        self.on_long_press = on_long_press

    def check(self) -> None:
        """timer triggered"""
        if self._active and (self.on_press or self.on_long_press):
            v = self.read()
            if v < self._threshold:  # button currently touched
                if not self._started:
                    self._started = ticks_ms()
                elif self._hold < ticks_diff(ticks_ms(), self._started):  # long press detected
                    if not self._acted:
                        self._active = False
                        self.on_long_press()
                        self._active = self._acted = True
            elif self._started:  # button was released
                if not self._acted:
                    self._active = False
                    self.on_press()
                    self._active = True
                else:
                    self._acted = False
                self._started = 0

    def enable(self):
        ct.register((self.check, self._freq))

    def disable(self):
        ct.unregister((self.check, self._freq))
