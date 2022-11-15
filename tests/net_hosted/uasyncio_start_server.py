# Test basic behaviour of uasyncio.start_server()

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit


async def test():
    # Test creating 2 servers using the same address
    print("create server1")
    server1 = await asyncio.start_server(None, "0.0.0.0", 8000)
    try:
        print("create server2")
        await asyncio.start_server(None, "0.0.0.0", 8000)
    except OSError as er:
        print("OSError")

    # Wait for server to close.
    async with server1:
        print("sleep")
        await asyncio.sleep(0)

    print("done")


asyncio.run(test())
