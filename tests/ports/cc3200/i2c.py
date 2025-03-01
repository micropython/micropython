"""
I2C test for the CC3200 based boards.
A MPU-9150 sensor must be connected to the I2C bus.
"""

from machine import I2C
import os
import time

mch = os.uname().machine
if "LaunchPad" in mch:
    i2c_pins = ("GP11", "GP10")
elif "WiPy" in mch:
    i2c_pins = ("GP15", "GP10")
else:
    raise Exception("Board not supported!")

i2c = I2C(0, I2C.MASTER, baudrate=400000)
# try initing without the peripheral id
i2c = I2C()
print(i2c)
i2c = I2C(mode=I2C.MASTER, baudrate=50000, pins=i2c_pins)
print(i2c)

i2c = I2C(0, I2C.MASTER, baudrate=100000)
print(i2c)
i2c = I2C(0, mode=I2C.MASTER, baudrate=400000)
print(i2c)
i2c = I2C(0, mode=I2C.MASTER, baudrate=400000, pins=i2c_pins)
print(i2c)

addr = i2c.scan()[0]
print(addr)

reg = bytearray(1)
reg2 = bytearray(2)
reg2_r = bytearray(2)

# reset the sensor
reg[0] |= 0x80
print(1 == i2c.writeto_mem(addr, 107, reg))
time.sleep_ms(100)  # wait for the sensor to reset...

print(1 == i2c.readfrom_mem_into(addr, 107, reg))  # read the power management register 1
print(0x40 == reg[0])

# now just read one byte
data = i2c.readfrom_mem(addr, 117, 1)  # read the "who am I?" register
print(0x68 == data[0])
print(len(data) == 1)
print(1 == i2c.readfrom_mem_into(addr, 117, reg))  # read the "who am I?" register again
print(0x68 == reg[0])

# now try reading two bytes
data = i2c.readfrom_mem(addr, 116, 2)  # read the "who am I?" register
print(0x68 == data[1])
print(data == b"\x00\x68")
print(len(data) == 2)
print(2 == i2c.readfrom_mem_into(addr, 116, reg2))  # read the "who am I?" register again
print(0x68 == reg2[1])
print(reg2 == b"\x00\x68")

print(1 == i2c.readfrom_mem_into(addr, 107, reg))  # read the power management register 1
print(0x40 == reg[0])
# clear the sleep bit
reg[0] = 0
print(1 == i2c.writeto_mem(addr, 107, reg))
# read it back
i2c.readfrom_mem_into(addr, 107, reg)
print(0 == reg[0])

# set the sleep bit
reg[0] = 0x40
print(1 == i2c.writeto_mem(addr, 107, reg))
# read it back
i2c.readfrom_mem_into(addr, 107, reg)
print(0x40 == reg[0])

# reset the sensor
reg[0] |= 0x80
print(1 == i2c.writeto_mem(addr, 107, reg))
time.sleep_ms(100)  # wait for the sensor to reset...

# now read and write two register at a time
print(2 == i2c.readfrom_mem_into(addr, 107, reg2))
print(0x40 == reg2[0])
print(0x00 == reg2[1])
# clear the sleep bit
reg2[0] = 0
# set some other bits
reg2[1] |= 0x03
print(2 == i2c.writeto_mem(addr, 107, reg2))
# read it back
i2c.readfrom_mem_into(addr, 107, reg2_r)
print(reg2 == reg2_r)

# reset the sensor
reg[0] = 0x80
print(1 == i2c.writeto_mem(addr, 107, reg))
time.sleep_ms(100)  # wait for the sensor to reset...

# try some raw read and writes
reg[0] = 117  # register address
print(1 == i2c.writeto(addr, reg, stop=False))  # just write the register address
# now read
print(1 == i2c.readfrom_into(addr, reg))
print(reg[0] == 0x68)
reg[0] = 117  # register address
print(1 == i2c.writeto(addr, reg, stop=False))  # just write the register address
# now read
print(0x68 == i2c.readfrom(addr, 1)[0])

i2c.readfrom_mem_into(addr, 107, reg2)
print(0x40 == reg2[0])
print(0x00 == reg2[1])

reg2[0] = 107  # register address
reg2[1] = 0
print(2 == i2c.writeto(addr, reg2, stop=True))  # write the register address and the data
i2c.readfrom_mem_into(addr, 107, reg)  # check it back
print(reg[0] == 0)

# check for memory leaks...
for i in range(0, 1000):
    i2c = I2C(0, I2C.MASTER, baudrate=100000)

# test deinit
i2c = I2C(0, I2C.MASTER, baudrate=100000)
i2c.deinit()
print(i2c)

# next ones should raise
try:
    i2c.scan()
except Exception:
    print("Exception")

try:
    i2c.readfrom(addr, 1)
except Exception:
    print("Exception")

try:
    i2c.readfrom_into(addr, reg)
except Exception:
    print("Exception")

try:
    i2c.readfrom_mem_into(addr, 107, reg)
except Exception:
    print("Exception")

try:
    i2c.writeto(addr, reg, stop=False)
except Exception:
    print("Exception")

try:
    i2c.writeto_mem(addr, 107, reg)
except Exception:
    print("Exception")

try:
    i2c.readfrom_mem(addr, 116, 2)
except Exception:
    print("Exception")

try:
    I2C(1, I2C.MASTER, baudrate=100000)
except Exception:
    print("Exception")

# reinitialization must work
i2c.init(baudrate=400000)
print(i2c)
