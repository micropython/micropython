# Test machine.SPITarget against machine.SPI, using two PSOC Edge boards.
#
# Connections on KIT_PSE84_AI (SCB10), between the two boards:
#   P16_0 (SCK), P16_1 (MOSI), P16_2 (MISO), P16_3 (SSEL/CS) and GND.
#
# instance0 is the SPI target, instance1 is the SPI controller.  Before each
# transfer, the target broadcasts that it is ready and the controller waits.

from machine import Pin, SPI, SPITarget

SPI_ID = 10
CS_PIN = "P16_3"
LONG_DATA = bytes((i * 7 + 3) & 0xFF for i in range(1037))


def instance0():
    target = SPITarget(SPI_ID)
    multitest.next()

    # Receive from the controller.
    rx = bytearray(4)
    multitest.broadcast("readinto")
    print("readinto:", target.readinto(rx), bytes(rx))

    # Send to the controller.  The data received meanwhile must be discarded,
    # so that the next readinto() gets the next data from the controller.
    multitest.broadcast("write")
    print("write:", target.write(b"\xaa\xbb\xcc\xdd"))
    multitest.broadcast("readinto after write")
    print("readinto after write:", target.readinto(rx), bytes(rx))

    # Send and receive at the same time.
    multitest.broadcast("write_readinto")
    print("write_readinto:", target.write_readinto(b"\x11\x22\x33\x44", rx), bytes(rx))

    # Receive more data than the FIFO can hold.
    rx = bytearray(len(LONG_DATA))
    multitest.broadcast("long readinto")
    print("long readinto:", target.readinto(rx), rx == LONG_DATA)

    # The controller sends more data than the FIFO can hold before readinto()
    # is called: the overflow is reported and the next transfer works.
    multitest.wait("data sent")
    try:
        target.readinto(bytearray(len(LONG_DATA)))
    except OSError as e:
        print("overflow:", e)
    rx = bytearray(4)
    multitest.broadcast("readinto after overflow")
    print("readinto after overflow:", target.readinto(rx), bytes(rx))

    target.deinit()


def instance1():
    cs = Pin(CS_PIN, Pin.OUT, value=1)
    spi = SPI(SPI_ID, baudrate=1_000_000)
    multitest.next()

    def transfer(step, fn, *args):
        if step:
            multitest.wait(step)
        cs(0)
        fn(*args)
        cs(1)

    transfer("readinto", spi.write, b"\x01\x02\x03\x04")

    rx = bytearray(4)
    transfer("write", spi.readinto, rx)
    print("readinto:", bytes(rx))
    transfer("readinto after write", spi.write, b"\x05\x06\x07\x08")

    transfer("write_readinto", spi.write_readinto, b"\xde\xad\xbe\xef", rx)
    print("write_readinto:", bytes(rx))

    transfer("long readinto", spi.write, LONG_DATA)

    transfer(None, spi.write, LONG_DATA)
    multitest.broadcast("data sent")
    transfer("readinto after overflow", spi.write, b"\x09\x0a\x0b\x0c")

    spi.deinit()
