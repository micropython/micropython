from pyb import SPI

# test we can correctly create by id or name
for bus in (-1, 0, 1, 2, 3, "X", "Y", "Z"):
    try:
        SPI(bus)
        print("SPI", bus)
    except ValueError:
        print("ValueError", bus)

spi = SPI(1)
print(spi)

spi = SPI(1, SPI.MASTER)
spi = SPI(1, SPI.MASTER, baudrate=500000)
spi = SPI(1, SPI.MASTER, 500000, polarity=1, phase=0, bits=8, firstbit=SPI.MSB, ti=False, crc=None)
print(spi)

spi.init(SPI.SLAVE, phase=1)
print(spi)
try:
    # need to flush input before we get an error (error is what we want to test)
    for i in range(10):
        spi.recv(1, timeout=100)
except OSError:
    print("OSError")

spi.init(SPI.MASTER)
spi.send(1, timeout=100)
print(spi.recv(1, timeout=100))
print(spi.send_recv(1, timeout=100))

spi.deinit()
