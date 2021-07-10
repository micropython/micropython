# Test TCP server with client issuing TCP RST part way through read

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit

import struct, time, socket

PORT = 8000


async def handle_connection(reader, writer):
    data = await reader.read(10)  # should succeed
    print(data)
    await asyncio.sleep(0.2)  # wait for client to drop connection
    try:
        data = await reader.read(100)
        print(data)
        writer.close()
        await writer.wait_closed()
    except OSError as er:
        print("OSError", er.errno)
    ev.set()


async def main():
    global ev
    ev = asyncio.Event()
    server = await asyncio.start_server(handle_connection, "0.0.0.0", PORT)
    multitest.next()
    async with server:
        await asyncio.wait_for(ev.wait(), 10)


def instance0():
    multitest.globals(IP=multitest.get_network_ip())
    asyncio.run(main())


def instance1():
    if not hasattr(socket, "SO_LINGER"):
        multitest.skip()
    multitest.next()
    s = socket.socket()
    s.connect(socket.getaddrinfo(IP, PORT)[0][-1])
    lgr_onoff = 1
    lgr_linger = 0
    s.setsockopt(socket.SOL_SOCKET, socket.SO_LINGER, struct.pack("ii", lgr_onoff, lgr_linger))
    s.send(b"GET / HTTP/1.0\r\n\r\n")
    time.sleep(0.1)
    s.close()  # This issues a TCP RST since we've set the linger option
