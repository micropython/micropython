"""
IMU module for Arduino Nano BLE 33 SENSE (REV1 and REV2).

Example usage:

import time
import imu
from machine import Pin, I2C

bus = I2C(1, scl=Pin(15), sda=Pin(14))
imu = imu.IMU(bus)

while (True):
    print('Accelerometer: x:{:>8.3f} y:{:>8.3f} z:{:>8.3f}'.format(*imu.accel()))
    print('Gyroscope:     x:{:>8.3f} y:{:>8.3f} z:{:>8.3f}'.format(*imu.gyro()))
    print('Magnetometer:  x:{:>8.3f} y:{:>8.3f} z:{:>8.3f}'.format(*imu.magnet()))
    print("")
    time.sleep_ms(100)
"""

import time


class IMU:
    def __init__(self, bus):
        """Initializes Gyro, Accelerometer and Magnetometer using default values."""
        if 0x68 in bus.scan():
            from bmm150 import BMM150
            from bmi270 import BMI270

            magnet = BMM150(bus)
            self.imu = BMI270(bus, bmm_magnet=magnet)
        else:
            from lsm9ds1 import LSM9DS1

            self.imu = LSM9DS1(bus)

    def gyro(self):
        return self.imu.gyro()

    def accel(self):
        return self.imu.accel()

    def magnet(self):
        return self.imu.magnet()
