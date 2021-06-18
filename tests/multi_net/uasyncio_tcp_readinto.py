# Test uasyncio stream readinto() method using TCP server/client

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit

try:
    import uarray as array
except ImportError:
    try:
        import array
    except ImportError:
        print("SKIP")
        raise SystemExit

PORT = 8000


async def handle_connection(reader, writer):
    writer.write(b"ab")
    await writer.drain()

    writer.write(b"c")
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

    ba = bytearray(2)
    n = await reader.readinto(ba)
    print(n)
    print(ba[:n])

    a = array.array("b", [0, 0])
    n = await reader.readinto(a)
    print(n)
    print(a[:n])

    try:
        n = await reader.readinto(5)
    except TypeError as er:
        print("TypeError")

    try:
        n = await reader.readinto()
    except TypeError as er:
        print("TypeError")


def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    asyncio.run(tcp_server())


def instance1():
    multitest.next()
    asyncio.run(tcp_client())
