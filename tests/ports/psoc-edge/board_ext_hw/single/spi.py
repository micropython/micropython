# SPI single-board loopback test
# Hardware: short MOSI (P16_1) to MISO (P16_2)
#
# Tests SPI master basic functionality using loopback.

from machine import SPI, Pin

# Test 1: Create SPI instance
spi = SPI(
    baudrate=1_000_000,
    polarity=0,
    phase=0,
    bits=8,
    firstbit=SPI.MSB,
    sck="P16_0",
    mosi="P16_1",
    miso="P16_2",
)
print(spi)

# Test 2: write_readinto loopback
tx = b"\xa5\x5a\xff\x00"
rx = bytearray(4)
spi.write_readinto(tx, rx)
print("write_readinto loopback: ", rx == b"\xa5\x5a\xff\x00")

# Test 3: write_readinto loopback (different data)
tx = b"\xde\xad\xbe\xef"
rx = bytearray(4)
spi.write_readinto(tx, rx)
print("transfer loopback: ", rx == b"\xde\xad\xbe\xef")

# Test 4: write (no read, just verify no crash)
spi.write(b"\x01\x02\x03")
print("write ok: ", True)

# Test 5: readinto (MISO reads what MOSI sends, but MOSI is idle so expect 0s)
rx = bytearray(4)
spi.readinto(rx)
print("readinto ok: ", True)

# Test 6: Different polarity/phase modes
spi.deinit()
for pol in (0, 1):
    for pha in (0, 1):
        s = SPI(
            baudrate=1_000_000,
            polarity=pol,
            phase=pha,
            bits=8,
            firstbit=SPI.MSB,
            sck="P16_0",
            mosi="P16_1",
            miso="P16_2",
        )
        tx = b"\xaa\x55"
        rx = bytearray(2)
        s.write_readinto(tx, rx)
        print("mode({},{}): ".format(pol, pha), rx == b"\xaa\x55")
        s.deinit()

# Test 7: LSB first
spi = SPI(
    baudrate=1_000_000,
    polarity=0,
    phase=0,
    bits=8,
    firstbit=SPI.LSB,
    sck="P16_0",
    mosi="P16_1",
    miso="P16_2",
)
tx = b"\xf0\x0f"
rx = bytearray(2)
spi.write_readinto(tx, rx)
print("LSB first loopback: ", rx == b"\xf0\x0f")
spi.deinit()

# Test 8: CS pin toggling with transfer
spi = SPI(
    baudrate=1_000_000,
    polarity=0,
    phase=0,
    bits=8,
    firstbit=SPI.MSB,
    sck="P16_0",
    mosi="P16_1",
    miso="P16_2",
)
cs = Pin("P16_3", Pin.OUT, value=1)
cs(0)
tx = b"\x9f\x00\x00\x00"
rx = bytearray(4)
spi.write_readinto(tx, rx)
cs(1)
print("CS toggled transfer: ", rx == b"\x9f\x00\x00\x00")
spi.deinit()

print("done")
