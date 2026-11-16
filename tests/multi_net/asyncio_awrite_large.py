# Test asyncio Stream.awrite()/drain() with large writes that force partial
# (would-block) writes.
#

try:
    import asyncio
    import asyncio.stream
except ImportError:
    raise
    print("SKIP")
    raise SystemExit

import sys
import gc
import struct
import socket

PORT = 8000

# SO_SNDBUF is not exported by the unix socket module; on Linux its value is 7.
# Used only to shrink the host's large loopback send buffer so the payload
# overflows it (forcing partial writes). Skipped elsewhere: on microcontrollers
# the native lwIP send buffer is already small, so partials occur naturally.
_SO_SNDBUF = 7


def shrink_sendbuf(sock):
    if sys.platform != "linux":
        return
    try:
        sock.setsockopt(socket.SOL_SOCKET, _SO_SNDBUF, struct.pack("i", 2048))
    except OSError:
        pass


# Period-257 repeating pattern (defeats naive reordering/duplication).
_UNIT = bytes(range(256)) + b"X"


def make_payload(n):
    return _UNIT * (n // 257) + _UNIT[: n % 257]


# ---------------------------------------------------------------------------
# instance0: server / receiver
# ---------------------------------------------------------------------------


async def handle_connection(reader, writer):
    # Brief pause before draining: keeps the sender's send buffer full so its
    # write() returns partial/None results -- i.e. exercises awrite()'s loop
    # (and the former drain() recursion) rather than letting one giant write
    # sail through the kernel buffer in a single call.
    await asyncio.sleep(0.05)

    for label in ("awrite", "drain"):
        try:
            data = await reader.readexactly(SIZE)
        except EOFError:
            print("recv", label, False)
            continue
        print("recv", label, data == make_payload(SIZE))

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


def instance0():
    global SIZE
    SIZE = gc.mem_free() // 10
    if SIZE < 4096:
        SIZE = 4096
    multitest.globals(IP=multitest.get_network_ip(), SIZE=SIZE)
    asyncio.run(tcp_server())


# ---------------------------------------------------------------------------
# instance1: client / sender
# ---------------------------------------------------------------------------


async def tcp_client():
    reader, writer = await asyncio.open_connection(IP, PORT)
    shrink_sendbuf(writer.s)

    # Heartbeat that yields every iteration. If awrite()/drain() cooperates
    # with the scheduler under backpressure, it advances *during* the sends.
    count = [0]
    running = [True]

    async def heartbeat():
        while running[0]:
            count[0] += 1
            await asyncio.sleep_ms(0)

    hb = asyncio.create_task(heartbeat())
    await asyncio.sleep_ms(0)

    before = count[0]
    await writer.awrite(make_payload(SIZE))
    writer.write(make_payload(SIZE))
    await writer.drain()
    after = count[0]

    running[0] = False
    await hb

    print("yielded", after - before > 0)
    print("awrote")
    print("drained")

    writer.close()
    await writer.wait_closed()


def instance1():
    multitest.next()
    asyncio.run(tcp_client())
