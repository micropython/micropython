import board
import busio

i2c = busio.I2C(board.SCL, board.SDA)
count = 0

# Wait for I2C lock
while not i2c.try_lock():
    pass

# Scan for devices on the I2C bus
print("Scanning I2C bus")
for x in i2c.scan():
    print(hex(x))
    count += 1

print("%d device(s) found on I2C bus" % count)

# Release the I2C bus
i2c.unlock()
