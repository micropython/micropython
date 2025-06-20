from pyb import I2C

i2c = I2C("I2C",baudrate=400000)

addr = i2c.scan()[0]

i2c.send(0, addr=addr)

data = i2c.recv(2, addr=addr)

temperature = int.from_bytes(data, 'big') >> 5

if temperature & 0x400 == 0x400:
    temperature = 0xFFF - temperature

temperature = temperature * 0.125

print(temperature)