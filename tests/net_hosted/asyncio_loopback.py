# Test network loopback behaviour

import sys

# Only certain platforms can do TCP/IP loopback.
if sys.platform not in ("darwin", "esp32", "linux"):
    print("SKIP")
    raise SystemExit

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def client(host, port):
    print(f"client open_connection to {host}:{port}")
    reader, writer = await asyncio.open_connection(host, port)

    data_in = b"A" * 100

    print("client writing")
    writer.write(data_in)
    await writer.drain()

    await asyncio.sleep(0.1)

    print("client reading")
    data = await reader.readexactly(100)
    print(f"client got {len(data)} bytes")

    assert data_in == data

    print("client closing")

    writer.close()
    await writer.wait_closed()

    print("client closed")


async def echo_handler(reader, writer):
    print("handler reading")
    await asyncio.sleep(0.1)
    data = await reader.readexactly(100)
    print(f"handler got {len(data)} bytes")

    print("handler writing")
    writer.write(data)
    await writer.drain()

    print("handler closing")

    writer.close()
    await writer.wait_closed()

    print("handler closed")


async def test(host, port):
    print(f"create server on {host}:{port}")
    server = await asyncio.start_server(echo_handler, host, port)

    async with server:
        print("server started")
        await client("127.0.0.1", 8080)
    print("server closed")


asyncio.run(test("0.0.0.0", 8080))
