# Test asyncio Stream.awrite() delivers a short payload exactly, over a real
# TCP pair (server/client) using the tests/multi_net idiom.

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit

PORT = 8000

MSG = b"hello awrite"


async def handle_connection(reader, writer):
    data = await reader.readexactly(len(MSG))
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
    await writer.awrite(MSG)
    print("awrote")
    writer.close()
    await writer.wait_closed()


def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    asyncio.run(tcp_server())


def instance1():
    multitest.next()
    asyncio.run(tcp_client())
