# Test that consecutive asyncio Stream.awrite() calls arrive in order, over a
# real TCP pair (server/client) using the tests/multi_net idiom.

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit

PORT = 8000

MSG1 = b"AAAA"
MSG2 = b"BBBB"


async def handle_connection(reader, writer):
    data = await reader.readexactly(len(MSG1) + len(MSG2))
    print("recv:", data)
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
    await writer.awrite(MSG1)
    print("awrite:", MSG1)
    await writer.awrite(MSG2)
    print("awrite:", MSG2)
    writer.close()
    await writer.wait_closed()


def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    asyncio.run(tcp_server())


def instance1():
    multitest.next()
    asyncio.run(tcp_client())
