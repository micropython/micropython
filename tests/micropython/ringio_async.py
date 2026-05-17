# Check that micropython.RingIO works correctly with asyncio.Stream.

try:
    import asyncio, micropython

    asyncio.StreamWriter
    micropython.RingIO
except (AttributeError, ImportError):
    print("SKIP")
    raise SystemExit

rb = micropython.RingIO(16)
rba = asyncio.StreamWriter(rb)

data = b"ABC123" * 20
print("w", len(data))


async def data_writer():
    global data
    rba.write(data)
    await rba.drain()


async def main():
    task = asyncio.create_task(data_writer())
    await asyncio.sleep_ms(10)
    read = await rba.readexactly(len(data))
    print(read)
    print("r", len(read))
    print(read == data)


asyncio.run(main())
