from machine import I2C


class Flashlight:
    ADDR = 0x48

    def __init__(self, i2c: I2C):
        self._i2c = i2c

    def begin(self):
        try:
            self.off()
            return True
        except OSError:
            return False

    def on(self) -> None:
        self._i2c.writeto(self.ADDR, bytearray([1]))

    def off(self) -> None:
        self._i2c.writeto(self.ADDR, bytearray([0]))
