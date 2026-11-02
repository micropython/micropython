"""async_spi transfers run while other asyncio tasks run, and can be cancelled.

Wiring (Raspberry Pi Pico / Pico 2): SPI1 on GP10 (SCK), GP11 (MOSI), GP12 (MISO),
with GP11 jumpered to GP12. Needs the asyncio library in CIRCUITPY/lib.
"""

import asyncio

import async_spi
import board


async def count(counter):
    while True:
        counter[0] += 1
        await asyncio.sleep(0)


async def main():
    with async_spi.SPI(board.GP10, MOSI=board.GP11, MISO=board.GP12) as spi:
        await spi.configure(baudrate=1_000_000)
        out = bytes(i * 7 & 0xFF for i in range(4096))
        inb = bytearray(len(out))

        # Another task runs while the transfer is on the wire, and the data comes back.
        counter = [0]
        counter_task = asyncio.create_task(count(counter))
        await spi.write_readinto(out, inb)
        counter_task.cancel()
        print("other task ran", counter[0], "times during a 33 ms transfer")
        assert counter[0] > 0
        assert inb == out

        await spi.readinto(inb, end=100, write_value=0xA5)
        assert inb[:100] == b"\xa5" * 100

        # A cancelled transfer stops, and the bus works afterwards.
        task = asyncio.create_task(spi.write(out))
        await asyncio.sleep(0.005)
        task.cancel()
        try:
            await task
        except asyncio.CancelledError:
            pass
        await spi.write_readinto(out, inb)
        assert inb == out
    print("PASS")


asyncio.run(main())
