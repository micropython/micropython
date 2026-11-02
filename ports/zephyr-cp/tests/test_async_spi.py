# SPDX-FileCopyrightText: 2026 Vladimir Smitka
# SPDX-License-Identifier: MIT

"""async_spi.SPI on native_sim, through the SPI loopback device."""

from pathlib import Path

import pytest

# The asyncio library is not frozen on native_sim, so the test puts it on the drive.
FROZEN = Path(__file__).resolve().parents[3] / "frozen"
ASYNCIO_LIB = {
    f"lib/asyncio/{path.name}": path.read_text()
    for path in (FROZEN / "Adafruit_CircuitPython_asyncio" / "asyncio").glob("*.py")
}
ASYNCIO_LIB["lib/adafruit_ticks.py"] = (
    FROZEN / "Adafruit_CircuitPython_Ticks" / "adafruit_ticks.py"
).read_text()

LOOPBACK_CODE = """\
import asyncio
import async_spi
import microcontroller


async def ticker(count):
    while True:
        count[0] += 1
        await asyncio.sleep(0)


async def main():
    spi = async_spi.SPI(microcontroller.pin.P_02, MOSI=microcontroller.pin.P_03, MISO=microcontroller.pin.P_00)
    await spi.lock.__aenter__()   # held for the whole test, as busio's try_lock would be
    spi.configure(baudrate=1_000_000)
    print("frequency", spi.frequency)
    bad = 0
    for n in (1, 32, 4096):
        out = bytes(i * 7 & 0xFF for i in range(n))
        inb = bytearray(n)
        await spi.write_readinto(out, inb)
        if inb != out:
            bad += 1
            print("write_readinto", n, "differs")
        await spi.readinto(inb, write_value=0xA5)
        if inb != b"\\xa5" * n:
            bad += 1
            print("readinto", n, "differs")
        await spi.write(out)

    # Another task keeps running around the transfers.
    count = [0]
    task = asyncio.create_task(ticker(count))
    for _ in range(10):
        await spi.write_readinto(out, inb)
    await asyncio.sleep(0)
    task.cancel()
    print("ticker ran", count[0], "times")

    # A cancelled transfer leaves the bus usable.
    task = asyncio.create_task(spi.write(out))
    await asyncio.sleep(0)
    task.cancel()
    try:
        await task
    except asyncio.CancelledError:
        pass
    await spi.write_readinto(out, inb)
    if inb != out:
        bad += 1
        print("after cancel differs")

    try:
        await spi.write(123)
    except TypeError:
        print("bad argument raised")
    spi.deinit()
    try:
        await spi.write(out)
    except ValueError:
        print("deinit raised")
    with async_spi.SPI(microcontroller.pin.P_02, MOSI=microcontroller.pin.P_03, MISO=microcontroller.pin.P_00) as spi2:
        try:
            await spi2.readinto(inb, end=8)
        except RuntimeError:
            print("no lock raised")
        async with spi2.lock:
            await spi2.readinto(inb, end=8, write_value=0x5A)
        if inb[:8] != b"\\x5a" * 8:
            bad += 1
            print("second bus differs")
    print("bad", bad)
    print("done")


asyncio.run(main())
"""


@pytest.mark.circuitpy_drive({"code.py": LOOPBACK_CODE, **ASYNCIO_LIB})
def test_async_spi_loopback(circuitpython):
    """Awaited transfers come back through the loopback, with cancel and deinit."""
    circuitpython.wait_until_done()

    output = circuitpython.serial.all_output
    assert "differs" not in output
    # 10 transfers of 4 KB at 1 MHz take about 330 ms; the ticker runs many times in that.
    ticker = int(output.split("ticker ran ")[1].split()[0])
    assert ticker > 20, ticker
    assert "bad argument raised" in output
    assert "deinit raised" in output
    assert "bad 0" in output
    assert "done" in output
