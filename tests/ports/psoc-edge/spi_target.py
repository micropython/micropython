# Test machine.SPITarget constructor (id/pins), init() and deinit() behaviour.
# Runs on a single board and needs no external connections.
try:
    from machine import SPITarget, SPI, UART
except ImportError:
    print("SKIP")
    raise SystemExit

SPI_ID, SCK_PIN, MOSI_PIN, MISO_PIN, SSEL_PIN = (10, "P16_0", "P16_1", "P16_2", "P16_3")


def check(fn):
    try:
        fn()
        print("no error")
    except Exception as e:
        print(type(e).__name__, e)


# The id alone is enough: pins are resolved from the id.
target = SPITarget(SPI_ID)
print(target)

# Constructing the same id again returns the same object.
print(SPITarget(SPI_ID) is target)

# init() reconfigures the object.
target.init(polarity=1, phase=1, firstbit=SPITarget.LSB)
print(target)

# init() without arguments resets everything to defaults.
target.init()
print(target)
target.deinit()

# Matching id and pins.
target = SPITarget(SPI_ID, sck=SCK_PIN, mosi=MOSI_PIN, miso=MISO_PIN, ssel=SSEL_PIN)
print(target)
target.deinit()

# Pins only: the id is resolved from the pins.
target = SPITarget(sck=SCK_PIN, mosi=MOSI_PIN, miso=MISO_PIN, ssel=SSEL_PIN)
print(target)
target.deinit()

# Invalid id and pin combinations.
check(lambda: SPITarget("10"))
check(lambda: SPITarget(-1))
check(lambda: SPITarget(12))
check(lambda: SPITarget())
check(lambda: SPITarget(SPI_ID, sck="P17_0"))
check(lambda: SPITarget(6))

# Invalid parameters.
check(lambda: SPITarget(SPI_ID, polarity=2))
check(lambda: SPITarget(SPI_ID, phase=2))
check(lambda: SPITarget(SPI_ID, bits=16))
check(lambda: SPITarget(SPI_ID, firstbit=5))

# A failed construction releases the SCB, so the id can be used again.
target = SPITarget(SPI_ID)
print(target)

# A failed init() keeps the object and the id: init() can be retried.
check(lambda: target.init(bits=16))
check(lambda: target.init(sck="P17_0"))
target.init(polarity=1)
print(target)

# A failed constructor on a live id keeps the live object and its configuration.
check(lambda: SPITarget(SPI_ID, bits=16))
check(lambda: SPITarget(SPI_ID, sck="P17_0"))
print(target)

# Pins only while the id is live returns the same object.
print(SPITarget(sck=SCK_PIN, mosi=MOSI_PIN, miso=MISO_PIN, ssel=SSEL_PIN) is target)

# The SCB is shared with other peripherals.
check(lambda: SPI(SPI_ID))
check(lambda: UART(SPI_ID))
target.deinit()
spi = SPI(SPI_ID)
check(lambda: SPITarget(SPI_ID))
spi.deinit()

# After deinit() the id can be used again.
target = SPITarget(SPI_ID)
print(target)
target.deinit()
