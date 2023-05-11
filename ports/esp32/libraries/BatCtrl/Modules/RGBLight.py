from machine import I2C
import time


class RGBLight:
    ADDR = 0x19

    def __init__(self, i2c: I2C):
        self._i2c = i2c
        self._red = 0
        self._green = 0
        self._blue = 0

    def begin(self) -> bool:
        try:
            self._send()
            return True
        except OSError:
            return False

    def set_r(self, value: int):
        self._red = min(100, max(0, value))
        self._send()

    def set_g(self, value: int):
        self._green = min(100, max(0, value))
        self._send()

    def set_b(self, value: int):
        self._blue = min(100, max(0, value))
        self._send()

    def set(self, red: int, green: int, blue: int):
        self._red = min(100, max(0, red))
        self._green = min(100, max(0, green))
        self._blue = min(100, max(0, blue))
        self._send()

    def get_r(self) -> int:
        return self._red

    def get_g(self) -> int:
        return self._green

    def get_b(self) -> int:
        return self._blue

    def get(self) -> tuple:
        return self.get_r(), self.get_g(), self.get_b()

    def _scale(self, val: int):
        """Scales from [0-100] to [0-255]"""
        val = min(100, max(0, val))
        return int(round((val * 255) / 100))

    def _send(self):
        data = bytes([
            self._scale(self._green),
            self._scale(self._red),
            self._scale(self._blue)
        ])
        self._i2c.writeto(self.ADDR, data)
        time.sleep_ms(1)
