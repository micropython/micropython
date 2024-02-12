# Test asyncio TCP server and client with TLS, giving a cert required error.

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
    print("handle connection")
    try:
        data = await reader.read(100)
    except Exception as e:
        print(e)
    ev.set()


async def tcp_server():
    global ev

    server_ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    server_ctx.load_cert_chain(cert, key)
    server_ctx.verify_mode = ssl.CERT_REQUIRED
    server_ctx.load_verify_locations(cafile=cert)
    ev = asyncio.Event()
    server = await asyncio.start_server(handle_connection, "0.0.0.0", PORT, ssl=server_ctx)
    print("server running")
    multitest.next()
    async with server:
        await asyncio.wait_for(ev.wait(), 10)
        multitest.wait("finished")
        print("server done")


async def tcp_client(message):
    client_ctx = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    client_ctx.verify_mode = ssl.CERT_REQUIRED
    client_ctx.load_verify_locations(cafile=cafile)
    reader, writer = await asyncio.open_connection(
        IP, PORT, ssl=client_ctx, server_hostname="micropython.local"
    )
    try:
        print("write:", message)
        writer.write(message)
        print("drain")
        await writer.drain()
    except Exception as e:
        print(e)
    print("client done")
    multitest.broadcast("finished")


def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    asyncio.run(tcp_server())


def instance1():
    multitest.next()
    asyncio.run(tcp_client(b"client data"))
