from machine import SoftI2C, Pin

# Define the SCL and SDA pins
scl_pin = Pin('IO_I2C2_SCL')
sda_pin = Pin('IO_I2C2_SDA')

# Initialize Software I2C with the specified pins, frequency, and timeout
I2C = SoftI2C(scl=scl_pin, sda=sda_pin, freq=400000, timeout=50000)

# Example: Scanning for I2C devices
devices = I2C.scan()
print("I2C devices found:", devices)

# Read 6 bytes from memory of device at address 0x50, starting at address 2
read = I2C.readfrom_mem(devices[0], 2, 6)

# Write '123456' (as bytes) to memory of the device with address 0x50, starting at address 2
I2C.writeto_mem(devices[0], 2, b'123456')  # 'abc' is converted to a bytes object
print("Read mem:", read)