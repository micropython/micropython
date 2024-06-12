# Test asyncio TCP server and client using start_server() and open_connection()

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit

PORT = 8000


async def handle_connection(reader, writer):
    # Test that peername exists (but don't check its value, it changes)
    writer.get_extra_info("peername")

    data = await reader.read(11)  # leaving bytes unread triggers the error
    # data = await reader.read(13)  # reading everything
    print("echo:", data.replace(b"\r\n", b""))
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
    print("write:", message.replace(b"\r\n", b""))
    writer.write(message)
    await writer.drain()
    data = await reader.read()  # if the client doesn't read exactly or is fast
    # enough e.g.
    # data = await reader.read(11)  # this works on unix because is
    # fast enough, however esp32 fails.
    print("read:", data.replace(b"\r\n", b""))


def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    asyncio.run(tcp_server())


def instance1():
    multitest.next()
    asyncio.run(tcp_client(b"client data\r\n"))
