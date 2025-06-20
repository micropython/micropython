from machine import I2C

i2c = I2C("I2C",freq=400000)

addr = i2c.scan()[0]

i2c.writeto(addr, b'0')

data = i2c.readfrom(addr,2)

temperature = int.from_bytes(data, 'big') >> 5

if temperature & 0x400 == 0x400:
    temperature = 0xFFF - temperature

temperature = temperature * 0.125

print(temperature)