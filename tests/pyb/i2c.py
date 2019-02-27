import pyb
from pyb import I2C

# test we can correctly create by id or name
for bus in (-1, 0, 1, 2, 3, "X", "Y", "Z"):
    try:
        I2C(bus)
        print("I2C", bus)
    except ValueError:
        print("ValueError", bus)

i2c = I2C(1)

i2c.init(I2C.MASTER, baudrate=400000)
print(i2c.scan())
i2c.deinit()

# use accelerometer to test i2c bus

accel_addr = 76

pyb.Accel() # this will init the MMA for us
i2c.init(I2C.MASTER, baudrate=400000)

print(i2c.scan())
print(i2c.is_ready(accel_addr))

print(i2c.mem_read(1, accel_addr, 7, timeout=500))
i2c.mem_write(0, accel_addr, 0, timeout=500)

i2c.send(7, addr=accel_addr)
i2c.recv(1, addr=accel_addr)
