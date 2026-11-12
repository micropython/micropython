# Test that cancelling one task (read or write) on a bidirectional stream in
# IOQueue does not unregister or orphan the other task waiting on the same stream.

try:
    import asyncio, io, select
except ImportError:
    print("SKIP")
    raise SystemExit

from micropython import const

_MP_STREAM_POLL = const(3)
_MP_STREAM_GET_FILENO = const(10)
_MP_STREAM_POLL_RD = const(0x0001)
_MP_STREAM_POLL_WR = const(0x0004)


class MockDuplexStream(io.IOBase):
    def __init__(self):
        self.rx_buf = b""
        self.tx_ready = False
        self.tx_log = b""

    def ioctl(self, req, arg):
        if req == _MP_STREAM_GET_FILENO:
            return -1
        if req == _MP_STREAM_POLL:
            ret = 0
            if (arg & _MP_STREAM_POLL_RD) and self.rx_buf:
                ret |= _MP_STREAM_POLL_RD
            if (arg & _MP_STREAM_POLL_WR) and self.tx_ready:
                ret |= _MP_STREAM_POLL_WR
            return ret
        return -1

    def read(self, n=-1):
        if not self.rx_buf:
            return None
        if n < 0 or n >= len(self.rx_buf):
            r = self.rx_buf
            self.rx_buf = b""
            return r
        r = self.rx_buf[:n]
        self.rx_buf = self.rx_buf[n:]
        return r

    def write(self, buf):
        if not self.tx_ready:
            return None
        self.tx_log += bytes(buf)
        return len(buf)


async def reader(stream, n):
    print("reader waiting")
    data = await stream.readexactly(n)
    print("reader got:", data)
    return data


async def writer(stream, data):
    print("writer waiting")
    stream.write(data)
    await stream.drain()
    print("writer drained")


async def main():
    # Case 1: reader is waiting while writer times out and gets cancelled.
    # Reader must remain registered in IOQueue and wake up when rx_buf arrives.
    raw = MockDuplexStream()
    stream = asyncio.StreamReader(raw)

    r_task = asyncio.create_task(reader(stream, 4))
    await asyncio.sleep(0.01)

    try:
        await asyncio.wait_for(writer(stream, b"blocked"), 0.02)
    except asyncio.TimeoutError:
        print("writer timed out")

    raw.rx_buf = b"pong"
    try:
        res = await asyncio.wait_for(r_task, 0.05)
        print("case 1 ok:", res)
    except asyncio.TimeoutError:
        print("case 1 FAIL: reader was orphaned")

    # Case 2: writer is waiting while reader times out and gets cancelled.
    # Writer must remain registered in IOQueue and wake up when tx_ready becomes True.
    raw2 = MockDuplexStream()
    stream2 = asyncio.StreamReader(raw2)

    w_task = asyncio.create_task(writer(stream2, b"hello"))
    await asyncio.sleep(0.01)

    try:
        await asyncio.wait_for(reader(stream2, 4), 0.02)
    except asyncio.TimeoutError:
        print("reader timed out")

    raw2.tx_ready = True
    try:
        await asyncio.wait_for(w_task, 0.05)
        print("case 2 ok:", raw2.tx_log)
    except asyncio.TimeoutError:
        print("case 2 FAIL: writer was orphaned")

    # Case 3: same as case 1, but the writer is cancelled directly with cancel()
    # rather than via wait_for(), to check the fix is not specific to wait_for().
    raw3 = MockDuplexStream()
    stream3 = asyncio.StreamReader(raw3)

    r_task = asyncio.create_task(reader(stream3, 4))
    await asyncio.sleep(0.01)

    w_task = asyncio.create_task(writer(stream3, b"blocked"))
    await asyncio.sleep(0.01)
    w_task.cancel()
    try:
        await w_task
    except asyncio.CancelledError:
        print("writer cancelled")

    raw3.rx_buf = b"ping"
    try:
        res = await asyncio.wait_for(r_task, 0.05)
        print("case 3 ok:", res)
    except asyncio.TimeoutError:
        print("case 3 FAIL: reader was orphaned")


asyncio.run(main())
