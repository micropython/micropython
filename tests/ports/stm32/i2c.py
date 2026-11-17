try:
    from pyb import I2C
except ImportError:
    print("SKIP")
    raise SystemExit

# test we can correctly create by id
for bus in (-1, 0, 1):
    try:
        I2C(bus)
        print("I2C", bus)
    except ValueError:
        print("ValueError", bus)

i2c = I2C(1)

i2c.init(I2C.CONTROLLER, baudrate=400000)
print(i2c.scan())
i2c.deinit()
