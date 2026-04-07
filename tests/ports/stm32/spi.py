# Test pyb.SPI basic behaviour.

from pyb import SPI

# Test invalid SPI identifiers.
for bus in (-1, 0, 100, "doesnotexist"):
    try:
        SPI(bus)
    except ValueError:
        print("ValueError", bus)

# Find the first available SPI (assume there is one).
spi_id = None
for bus in (1, 2, 3):
    try:
        SPI(bus)
        spi_id = bus
        break
    except ValueError:
        pass


# Print SPI without id, baudrate or prescaler.
def print_spi(spi):
    s = str(spi)
    assert s.startswith("SPI(") and s.endswith(")")
    s = s[4:-1].split(", ")
    assert s[0].isdigit()
    del s[0]
    if len(s) > 1 and s[0] == "SPI.CONTROLLER":
        assert s[1].startswith("baudrate=")
        assert s[2].startswith("prescaler=")
        del s[1:3]
    print("SPI(", ", ".join(s), ")", sep="")


# Test constructing SPI with various parameters.
spi = SPI(spi_id)
print_spi(spi)
spi = SPI(spi_id, SPI.CONTROLLER)
print_spi(spi)
spi = SPI(spi_id, SPI.CONTROLLER, baudrate=500000)
print_spi(spi)
spi = SPI(
    spi_id,
    SPI.CONTROLLER,
    500000,
    polarity=1,
    phase=0,
    bits=8,
    firstbit=SPI.MSB,
    ti=False,
    crc=None,
)
print_spi(spi)

# Test SPI.PERIPHERAL mode.
spi.init(SPI.PERIPHERAL, phase=1)
print_spi(spi)
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
