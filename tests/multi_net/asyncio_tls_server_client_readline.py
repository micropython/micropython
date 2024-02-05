# Test asyncio TCP server and client with TLS, using readline() to read data.

try:
    import os
    import asyncio
    import ssl
except ImportError:
    print("SKIP")
    raise SystemExit

PORT = 8000

# These are test certificates. See tests/README.md for details.
cert = cafile = "ec_cert.der"
key = "ec_key.der"

try:
    os.stat(cafile)
    os.stat(key)
except OSError:
    print("SKIP")
    raise SystemExit


async def handle_connection(reader, writer):
    data = await reader.readline()
    print("echo:", data)
    data2 = await reader.readline()
    print("echo:", data2)
    writer.write(data + data2)
    await writer.drain()

    print("close")
    writer.close()
    await writer.wait_closed()

    print("done")
    ev.set()


async def tcp_server():
    global ev

    server_ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    server_ctx.load_cert_chain(cert, key)
    ev = asyncio.Event()
    server = await asyncio.start_server(handle_connection, "0.0.0.0", PORT, ssl=server_ctx)
    print("server running")
    multitest.next()
    async with server:
        await asyncio.wait_for(ev.wait(), 10)


async def tcp_client(message):
    client_ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    client_ctx.verify_mode = ssl.CERT_REQUIRED
    client_ctx.load_verify_locations(cafile=cafile)
    reader, writer = await asyncio.open_connection(
        IP, PORT, ssl=client_ctx, server_hostname="micropython.local"
    )
    print("write:", message)
    writer.write(message)
    await writer.drain()
    data = await reader.readline()
    print("read:", data)
    data2 = await reader.readline()
    print("read:", data2)


def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    asyncio.run(tcp_server())


def instance1():
    multitest.next()
    asyncio.run(tcp_client(b"client data\nclient data2\n"))
