### I2C
from machine import I2C

i2c = I2C(id=0, scl="P6_0", sda="P6_1", freq=400000)
print(i2c)

print(i2c.scan())
