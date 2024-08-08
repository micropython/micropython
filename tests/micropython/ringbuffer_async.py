# check that micropython.ringbuffer works correctly with asyncio.Stream.

import micropython

try:
    import asyncio

    asyncio.StreamWriter
    micropython.ringbuffer
except (AttributeError, ImportError):
    print("SKIP")
    raise SystemExit

rb = micropython.ringbuffer(16)
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
    # buff = bytearray(len(data))
    read = await rba.readexactly(len(data))
    print(read)
    print("r", len(read))
    print(read == data)


asyncio.run(main())
