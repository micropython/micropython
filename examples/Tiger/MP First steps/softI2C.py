from machine import SoftI2C,Pin

sda = Pin("SDA")
scl = Pin("SCL")

i2c = SoftI2C(scl=scl, sda=sda)

addr = 0x48

data = bytearray(6)

i2c.writeto(addr, data)

i2c.readfrom_into(addr,data)

temperature = int.from_bytes(data[0:2], 'big') >> 5

if temperature & 0x400 == 0x400:
    temperature = 0xFFF - temperature

temperature = temperature * 0.125

print(temperature)