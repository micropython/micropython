import math

from machine import I2C
from micropython import const


class LSM6DS3TR:
    LSM6DS3TR_C_ID = const(0x6A)
    REG_WHO_AM_I = const(0x0F)
    REG_CTRL1_XL = const(0x10)
    REG_CTRL2_G = const(0x11)
    REG_OUT_X_L_XL = const(0x28)
    REG_OUT_Y_L_XL = const(0x2A)
    REG_OUT_Z_L_XL = const(0x2C)
    REG_OUT_X_L_GY = const(0x22)
    REG_OUT_Y_L_GY = const(0x24)
    REG_OUT_Z_L_GY = const(0x26)

    # Linear acceleration is in m/s^2, angular velocity is in rad/s
    class Sample:
        gyroX: float
        gyroY: float
        gyroZ: float
        accelX: float
        accelY: float
        accelZ: float

        def __init__(self, gyroX, gyroY, gyroZ, accelX, accelY, accelZ):
            self.gyroX = gyroX
            self.gyroY = gyroY
            self.gyroZ = gyroZ
            self.accelX = accelX
            self.accelY = accelY
            self.accelZ = accelZ

    def __init__(self, i2c: I2C, addr=0x6A):
        self.i2c = i2c
        self.addr = addr

    def begin(self):
        try:
            if self.getreg(self.REG_WHO_AM_I) != self.LSM6DS3TR_C_ID:
                return False
        except OSError:
            return False

        # accelero ODR = 104Hz, scale = 16g
        self.setreg(self.REG_CTRL1_XL, 0x44)

        # gyro ODR = 104Ht, scale = 2000dps
        self.setreg(self.REG_CTRL2_G, 0x4C)

        return True

    def int16(self, d):
        return d if d < 0x8000 else d - 0x10000

    def setreg(self, reg: int, dat: int):
        self.i2c.writeto_mem(self.addr, reg, bytes([dat]))

    def getreg(self, reg: int):
        return self.i2c.readfrom_mem(self.addr, reg, 1)[0]

    def get2reg(self, reg: int):
        return self.getreg(reg) + self.getreg(reg + 1) * 256

    def get_xl_x(self) -> float:
        return self.convert_accel(self.REG_OUT_X_L_XL)

    def get_xl_y(self) -> float:
        return self.convert_accel(self.REG_OUT_Y_L_XL)

    def get_xl_z(self) -> float:
        return self.convert_accel(self.REG_OUT_Z_L_XL)

    def convert_accel(self, reg):
        return self.int16(self.get2reg(reg)) * 0.488 / 1000.0

    def convert_gyro(self, reg):
        return self.int16(self.get2reg(reg)) * 70.0 * math.pi / 180.0 / 1000.0

    def get_gy_x(self) -> float:
        return self.convert_gyro(self.REG_OUT_X_L_GY)

    def get_gy_y(self) -> float:
        return self.convert_gyro(self.REG_OUT_Y_L_GY)

    def get_gy_z(self) -> float:
        return self.convert_gyro(self.REG_OUT_Z_L_GY)

    def get_accel(self):
        x = self.get_xl_x()
        y = self.get_xl_y()
        z = self.get_xl_z()
        return x, y, z

    def get_gyro(self):
        x = self.get_gy_x()
        y = self.get_gy_y()
        z = self.get_gy_z()
        return x, y, z

    def get_sample(self):
        sample = self.Sample(self.get_gy_x(),
                             self.get_gy_y(),
                             self.get_gy_z(),
                             self.get_xl_x(),
                             self.get_xl_y(),
                             self.get_xl_z())
        return sample
