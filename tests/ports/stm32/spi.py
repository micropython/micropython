from pyb import SPI

# test we can correctly create by id
for bus in (-1, 0, 1, 2):
    try:
        SPI(bus)
        print("SPI", bus)
    except ValueError:
        print("ValueError", bus)

spi = SPI(1)
print(spi)

spi = SPI(1, SPI.CONTROLLER)
spi = SPI(1, SPI.CONTROLLER, baudrate=500000)
spi = SPI(
    1, SPI.CONTROLLER, 500000, polarity=1, phase=0, bits=8, firstbit=SPI.MSB, ti=False, crc=None
)
print(str(spi)[:32], str(spi)[53:])  # don't print baudrate/prescaler

spi.init(SPI.PERIPHERAL, phase=1)
print(spi)
try:
    # need to flush input before we get an error (error is what we want to test)
    for i in range(10):
        spi.recv(1, timeout=100)
except OSError:
    print("OSError")

spi.init(SPI.CONTROLLER)
spi.send(1, timeout=100)
print(spi.recv(1, timeout=100))
print(spi.send_recv(1, timeout=100))

spi.deinit()
