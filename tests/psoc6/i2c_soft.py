#### SoftI2C
from machine import SoftI2C

si2c = SoftI2C(scl="P6_0", sda="P6_1", freq=400000)
print(si2c)

print(si2c.scan())
