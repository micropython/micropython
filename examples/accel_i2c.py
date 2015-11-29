# This is an example on how to access accelerometer on
# PyBoard directly using I2C bus. As such, it's more
# intended to be an I2C example, rather than accelerometer
# example. For the latter, using pyb.Accel class is
# much easier.

from machine import Pin
from machine import I2C
import time

# Accelerometer needs to be powered on first. Even
# though signal is called "AVDD", and there's separate
# "DVDD", without AVDD, it won't event talk on I2C bus.
accel_pwr = Pin("MMA_AVDD")
accel_pwr.value(1)

i2c = I2C(1, baudrate=100000)
addrs = i2c.scan()
print("Scanning devices:", [hex(x) for x in addrs])
if 0x4c not in addrs:
    print("Accelerometer is not detected")

ACCEL_ADDR = 0x4c
ACCEL_AXIS_X_REG = 0
ACCEL_MODE_REG = 7

# Now activate measurements
i2c.mem_write(b"\x01", ACCEL_ADDR, ACCEL_MODE_REG)

print("Try to move accelerometer and watch the values")
while True:
    val = i2c.mem_read(1, ACCEL_ADDR, ACCEL_AXIS_X_REG)
    print(val[0])
    time.sleep(1)
