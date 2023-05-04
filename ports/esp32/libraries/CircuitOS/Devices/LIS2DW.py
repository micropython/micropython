from machine import I2C
from micropython import const


class LIS2DW:
    ADDR = const(0x18)

    def __init__(self, i2c: I2C, address: int = ADDR):
        self._i2c = i2c
        self._addr = address

    def begin(self) -> bool:
        data = bytearray([0x20, 0b01010001])

        try:
            self._i2c.writeto(self._addr, data)
            return True
        except OSError:
            return False

    def _read_data(self, register: int, length: int):
        self._i2c.writeto(self._addr, bytearray([register]))
        data = self._i2c.readfrom(self._addr, length)
        return data

    def get_accel(self) -> tuple:
        data = self._read_data(0x28, 6)

        x = (data[1] << 8) | data[0]
        y = (data[3] << 8) | data[2]
        z = (data[5] << 8) | data[4]

        return x, y, z

    def get_temp(self) -> float:
        temp_data = self._read_data(0x26, 1)
        temp = temp_data[0] + 25
        return temp

    def get_x(self) -> int:
        x, _, _ = self.get_accel()
        return x

    def get_y(self) -> int:
        _, y, _ = self.get_accel()
        return y

    def get_z(self) -> int:
        _, _, z = self.get_accel()
        return z
