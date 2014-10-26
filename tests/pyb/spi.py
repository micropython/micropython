from pyb import SPI

spi = SPI(1)
print(spi)

spi = SPI(1, SPI.MASTER)
spi = SPI(1, SPI.MASTER, baudrate=500000)
spi = SPI(1, SPI.MASTER, 500000, polarity=1, phase=0, bits=8, firstbit=SPI.MSB, ti=False, crc=None)
print(spi)

spi.init(SPI.SLAVE, phase=1)
print(spi)

spi.init(SPI.MASTER)
spi.send(1, timeout=100)
print(spi.recv(1, timeout=100))
print(spi.send_recv(1, timeout=100))
