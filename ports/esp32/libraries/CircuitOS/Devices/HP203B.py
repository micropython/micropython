from machine import I2C
import time
from micropython import const


class HP203B:
    ADDR = const(0x76)
    PRESSURE = const(0x30)
    ALTITUDE = const(0x31)

    def __init__(self, i2c: I2C, addr: int = ADDR):
        self._i2c = i2c
        self._addr = addr

    def begin(self) -> bool:
        try:
            self._i2c.writeto(self._addr, bytearray([0x06]))  # soft reset
            time.sleep_ms(5)

            self._i2c.writeto(self._addr, bytearray([0b01010100]))
            return True
        except OSError:
            return False

    def get_baro(self) -> int:
        return self._read_sensor(self.PRESSURE)

    def get_alt(self) -> int:
        return self._read_sensor(self.ALTITUDE)

    def _read_sensor(self, sensor: int) -> int:
        self._i2c.writeto(self._addr, bytearray([sensor]))

        data = self._i2c.readfrom(self._addr, 3)
        data_int = (data[0] << 16) | (data[1] << 8) | data[2]

        return data_int // 100
