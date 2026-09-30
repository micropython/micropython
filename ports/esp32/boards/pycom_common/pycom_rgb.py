"""
RGB LED (WS2812 NeoPixel) helper for Pycom boards.

Usage:
    from pycom_rgb import LED
    led = LED()
    led.color(0, 0, 255)   # Blue
    led.off()
    led.heartbeat()         # Pulsing blue heartbeat
"""

import time
from machine import Pin
from neopixel import NeoPixel


class LED:
    """Control the on-board WS2812 RGB LED on GPIO0."""

    def __init__(self, pin=0):
        self._np = NeoPixel(Pin(pin, Pin.OUT), 1)
        self.off()

    def color(self, r, g, b):
        """Set LED color (0-255 per channel)."""
        self._np[0] = (r, g, b)
        self._np.write()

    def off(self):
        """Turn LED off."""
        self.color(0, 0, 0)

    def heartbeat(self, color=(0, 0, 50), cycles=3):
        """
        Simple pulsing heartbeat effect.

        Args:
            color: Base (r, g, b) tuple
            cycles: Number of pulse cycles
        """
        r, g, b = color
        for _ in range(cycles):
            # Fade in
            for i in range(0, 101, 5):
                f = i / 100
                self.color(int(r * f), int(g * f), int(b * f))
                time.sleep_ms(20)
            # Fade out
            for i in range(100, -1, -5):
                f = i / 100
                self.color(int(r * f), int(g * f), int(b * f))
                time.sleep_ms(20)
            time.sleep_ms(200)
        self.off()
