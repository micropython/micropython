import board
import busio
import digitalio

cs = digitalio.DigitalInOut(board.SS)
cs.direction = digitalio.Direction.OUTPUT
cs.value = True

BME680_SPI_REGISTER = 0x73
BME680_CHIPID_REGISTER = 0xD0
BME680_CHIPID = 0x61
SPI_HERZ = 0x500000

spi = busio.SPI(board.SCK, MISO=board.MISO, MOSI=board.MOSI)


def readByte(addr):
    value = -1
    while not spi.try_lock():
        pass
    try:
        spi.configure(baudrate=500000, phase=0, polarity=0)

        cs.value = False
        result = bytearray(1)
        result[0] = addr | 0x80
        spi.write(result)
        spi.readinto(result)
        value = result[0]
        return value
    finally:
        spi.unlock()
        cs.value = True


def writeByte(addr, value):
    while not spi.try_lock():
        pass
    try:
        spi.configure(baudrate=500000, phase=0, polarity=0)

        cs.value = False
        result = bytearray(2)
        result[0] = addr & ~0x80
        result[1] = value
        spi.write(result)
    finally:
        spi.unlock()


# put the device in the correct mode to read the ID
reg = readByte(BME680_SPI_REGISTER)
if (reg & 16) != 0:
    writeByte(BME680_SPI_REGISTER, reg & ~16)

id = readByte(BME680_CHIPID_REGISTER)

print(f"id is {id}, expected {BME680_CHIPID}")
