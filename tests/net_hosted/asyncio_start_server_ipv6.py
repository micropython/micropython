# Test asyncio.start_server() with IPv6 address

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

PORT = 8000


async def handle_connection(reader, writer):
    data = await reader.read(100)
    print("echo:", data)
    writer.write(data)
    await writer.drain()

    print("close")
    writer.close()
    await writer.wait_closed()

    print("done")
    global server_done
    server_done = True


async def test_ipv6_server():
    global server_done
    server_done = False

    # Start server with IPv6 address
    print("create ipv6 server")
    server = await asyncio.start_server(handle_connection, "::", PORT)
    print("server running")

    try:
        # Connect with IPv6 client
        print("connect to ipv6 server")
        reader, writer = await asyncio.open_connection("::", PORT)

        # Send test data
        test_msg = b"ipv6 test data"
        print("write:", test_msg)
        writer.write(test_msg)
        await writer.drain()

        # Read response
        data = await reader.read(100)
        print("read:", data)
        assert data == test_msg, "Data mismatch"

        # Close client connection
        print("close client")
        writer.close()
        await writer.wait_closed()

        # Wait for server to complete handling
        while not server_done:
            await asyncio.sleep(0.1)

    finally:
        # Ensure server is closed
        print("close server")
        server.close()
        await server.wait_closed()

    print("test passed")


# Run the test
asyncio.run(test_ipv6_server())
