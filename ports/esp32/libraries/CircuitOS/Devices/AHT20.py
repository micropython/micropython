from machine import I2C
import time
from micropython import const


class AHT20:
    ADDR = const(0x38)

    def __init__(self, i2c: I2C):
        self._i2c = i2c

    def begin(self) -> bool:
        try:
            self._i2c.writeto(self.ADDR, bytearray([0x00]))
            return True
        except OSError:
            return False

    def _read_data(self):
        self._i2c.writeto(self.ADDR, bytearray([0xAC, 0x33, 0x00]))
        time.sleep_ms(80)
        data = self._i2c.readfrom(self.ADDR, 6)
        return data

    def get_hum(self) -> float:
        data = self._read_data()

        h = data[1] << 12 | data[2] << 4 | data[3] >> 4
        hum = (h * 100) / 0x100000

        return hum

    def get_temp(self) -> float:
        data = self._read_data()

        tdata = (data[3] & 0x0F) << 16 | data[4] << 8 | data[5]
        temp = (tdata * 200 / 0x100000) - 50

        return temp
