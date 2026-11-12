# Test machine.SPI constructor (id/pins), init() and deinit() behaviour.
# Runs on a single board and needs no external connections.
try:
    from machine import SPI, UART
except ImportError:
    print("SKIP")
    raise SystemExit

SPI_ID, SCK_PIN, MOSI_PIN, MISO_PIN = (10, "P16_0", "P16_1", "P16_2")


def check(fn):
    try:
        fn()
        print("no error")
    except Exception as e:
        print(type(e).__name__, e)


# The id alone is enough: pins are resolved from the id.
spi = SPI(SPI_ID)
print(spi)

# Constructing the same id again returns the same object.
print(SPI(SPI_ID) is spi)

# init() reconfigures the object. baudrate is the first positional argument.
spi.init(2_000_000, polarity=1, phase=1, firstbit=SPI.LSB)
print(spi)

# init() without arguments resets everything to defaults.
spi.init()
print(spi)
spi.deinit()

# Matching id and pins.
spi = SPI(SPI_ID, sck=SCK_PIN, mosi=MOSI_PIN, miso=MISO_PIN)
print(spi)
spi.deinit()

# Pins only: the id is resolved from the pins.
spi = SPI(sck=SCK_PIN, mosi=MOSI_PIN, miso=MISO_PIN)
print(spi)
spi.deinit()

# Invalid id and pin combinations.
check(lambda: SPI("10"))
check(lambda: SPI(-1))
check(lambda: SPI(12))
check(lambda: SPI())
check(lambda: SPI(SPI_ID, sck="P17_0"))
check(lambda: SPI(6))

# Invalid parameters.
check(lambda: SPI(SPI_ID, baudrate=0))
check(lambda: SPI(SPI_ID, polarity=2))
check(lambda: SPI(SPI_ID, phase=2))
check(lambda: SPI(SPI_ID, bits=16))
check(lambda: SPI(SPI_ID, firstbit=5))

# A failed construction releases the SCB, so the id can be used again.
spi = SPI(SPI_ID)
print(spi)

# A failed init() keeps the object and the id: init() can be retried.
check(lambda: spi.init(bits=16))
check(lambda: spi.init(sck="P17_0"))
spi.init(baudrate=500_000)
print(spi)

# A failed constructor on a live id keeps the live object and its configuration.
check(lambda: SPI(SPI_ID, bits=16))
check(lambda: SPI(SPI_ID, sck="P17_0"))
print(spi)

# The object is still functional.
buf = bytearray(4)
spi.write(b"\x00")
spi.readinto(buf)
spi.write_readinto(b"\x01\x02\x03\x04", buf)
print("transfers done")

# Pins only while the id is live returns the same object.
print(SPI(sck=SCK_PIN, mosi=MOSI_PIN, miso=MISO_PIN) is spi)

# The SCB is shared with other peripherals.
check(lambda: UART(SPI_ID))
spi.deinit()
uart = UART(SPI_ID)
check(lambda: SPI(SPI_ID))
uart.deinit()

# After deinit() the id can be used again.
spi = SPI(SPI_ID)
print(spi)
spi.deinit()
