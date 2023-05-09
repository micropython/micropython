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
            self._i2c.writeto(self.ADDR, bytearray([self._red, self._green, self._blue]))
            return True
        except OSError:
            return False

    def set_r(self, value: int):
        self._red = value & 0xFF
        self._i2c.writeto(self.ADDR, bytearray([self._red, self._green, self._blue]))
        time.sleep_ms(1)

    def set_g(self, value: int):
        self._green = value & 0xFF
        self._i2c.writeto(self.ADDR, bytearray([self._red, self._green, self._blue]))
        time.sleep_ms(1)

    def set_b(self, value: int):
        self._blue = value & 0xFF
        self._i2c.writeto(self.ADDR, bytearray([self._red, self._green, self._blue]))
        time.sleep_ms(1)

    def set(self, red: int, green: int, blue: int):
        self._red = red & 0xFF
        self._green = green & 0xFF
        self._blue = blue & 0xFF
        self._i2c.writeto(self.ADDR, bytearray([self._red, self._green, self._blue]))

    def get_r(self) -> int:
        return self._red

    def get_g(self) -> int:
        return self._green

    def get_b(self) -> int:
        return self._blue

    def get(self) -> tuple:
        return self.get_r(), self.get_g(), self.get_b()
