# Test asyncio stream readexactly() method using TCP server/client

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit

PORT = 8000


async def handle_connection(reader, writer):
    writer.write(b"a")
    await writer.drain()

    # Split the first 2 bytes up so the client must wait for the second one
    await asyncio.sleep(1)

    writer.write(b"b")
    await writer.drain()

    writer.write(b"c")
    await writer.drain()

    writer.write(b"d")
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


async def tcp_client():
    reader, writer = await asyncio.open_connection(IP, PORT)
    print(await reader.readexactly(2))
    print(await reader.readexactly(0))
    print(await reader.readexactly(1))
    try:
        print(await reader.readexactly(2))
    except EOFError as er:
        print("EOFError")
    print(await reader.readexactly(0))


def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    asyncio.run(tcp_server())


def instance1():
    multitest.next()
    asyncio.run(tcp_client())
