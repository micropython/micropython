# Test asyncio TCP server and client using start_server() and open_connection() with IPv6

try:
    import asyncio
    import socket
except ImportError:
    print("SKIP")
    raise SystemExit

try:
    # Check if IPv6 is supported
    socket.socket(socket.AF_INET6, socket.SOCK_STREAM)
except (AttributeError, OSError):
    print("SKIP")
    raise SystemExit

PORT = 8001  # Different from other tests to avoid conflicts


async def handle_connection(reader, writer):
    # Test that peername exists
    peer = writer.get_extra_info("peername")
    print("peer connected")

    data = await reader.read(100)
    print("echo:", data)
    writer.write(data)
    await writer.drain()

    print("close")
    writer.close()
    await writer.wait_closed()

    print("done")
    ev.set()


async def tcp_server_ipv6():
    global ev
    ev = asyncio.Event()

    # Start server with IPv6 address
    server = await asyncio.start_server(handle_connection, "::", PORT)
    print("ipv6 server running")
    multitest.next()

    async with server:
        await asyncio.wait_for(ev.wait(), 10)


async def tcp_client_ipv6(message):
    # Connect to the IPv6 server
    reader, writer = await asyncio.open_connection(IPV6, PORT)
    print("write:", message)
    writer.write(message)
    await writer.drain()

    data = await reader.read(100)
    print("read:", data)
    assert data == message, "Data mismatch"


def instance0():
    # Get the IPv6 address using the new parameter
    ipv6 = multitest.get_network_ip(ipv6=True)
    multitest.globals(IPV6=ipv6)
    asyncio.run(tcp_server_ipv6())


def instance1():
    multitest.next()
    asyncio.run(tcp_client_ipv6(b"ipv6 client data"))
