# Test uasyncio TCP server and client using start_server() and open_connection()

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
    # Test that peername exists (but don't check its value, it changes)
    writer.get_extra_info("peername")

    data = await reader.read(100)
    print("echo:", data)
    writer.write(data)
    await writer.drain()

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
        await asyncio.wait_for(ev.wait(), 10)


async def tcp_client(message):
    reader, writer = await asyncio.open_connection(IP, PORT)
    print("write:", message)
    writer.write(message)
    await writer.drain()
    data = await reader.read(100)
    print("read:", data)


def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    asyncio.run(tcp_server())


def instance1():
    multitest.next()
    asyncio.run(tcp_client(b"client data"))
