# LSM6DSOX Basic Example.
import time
from lsm6dsox import LSM6DSOX

from machine import Pin, I2C

lsm = LSM6DSOX(I2C(0, scl=Pin(13), sda=Pin(12)))

while True:
    print("Accelerometer: x:{:>8.3f} y:{:>8.3f} z:{:>8.3f}".format(*lsm.read_accel()))
    print("Gyroscope:     x:{:>8.3f} y:{:>8.3f} z:{:>8.3f}".format(*lsm.read_gyro()))
    print("")
    time.sleep_ms(100)
