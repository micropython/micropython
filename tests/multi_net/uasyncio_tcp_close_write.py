# Test uasyncio TCP stream closing then writing

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit

PORT = 8000


async def handle_connection(reader, writer):
    # Write data to ensure connection
    writer.write(b"x")
    await writer.drain()

    # Read, should return nothing
    print("read:", await reader.read(100))

    # Close connection
    print("close")
    writer.close()
    await writer.wait_closed()

    print("done")
    ev.set()


async def tcp_server():
    global ev
    ev = asyncio.Event()
    server = await asyncio.start_server(handle_connection, "0.0.0.0", PORT)
    print("server running")
    multitest.next()
    async with server:
        await asyncio.wait_for(ev.wait(), 5)


async def tcp_client():
    reader, writer = await asyncio.open_connection(IP, PORT)

    # Read data to ensure connection
    print("read:", await reader.read(1))

    # Close connection
    print("close")
    writer.close()
    await writer.wait_closed()

    # Try writing data to the closed connection
    print("write")
    try:
        writer.write(b"x")
        await writer.drain()
    except OSError:
        print("OSError")


def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    asyncio.run(tcp_server())


def instance1():
    multitest.next()
    asyncio.run(tcp_client())
