# Test cancelling a task waiting on stream IO

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit


async def get(reader):
    print("start")
    try:
        await reader.read(10)
        print("fail")
    except asyncio.CancelledError:
        print("cancelled")


async def main(url):
    reader, writer = await asyncio.open_connection(url, 80)
    task = asyncio.create_task(get(reader))
    await asyncio.sleep(0)
    print("cancelling")
    task.cancel()
    print("waiting")
    await task
    print("done")
    writer.close()
    await writer.wait_closed()


asyncio.run(main("micropython.org"))
