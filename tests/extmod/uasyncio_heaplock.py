# test that the following do not use the heap:
# - basic scheduling of tasks
# - uasyncio.sleep_ms
# - StreamWriter.write, stream is blocked and data to write is a bytes object
# - StreamWriter.write, when stream is not blocked

import micropython

# strict stackless builds can't call functions without allocating a frame on the heap
try:
    # force bytecode (in case we're running with emit=native) and verify
    # that bytecode-calling-bytecode doesn't allocate
    @micropython.bytecode
    def f(x):
        x and f(x - 1)

    micropython.heap_lock()
    f(1)
    micropython.heap_unlock()
except RuntimeError:
    # RuntimeError (max recursion depth) not MemoryError because effectively
    # the recursion depth is at the limit while the heap is locked with
    # stackless
    print("SKIP")
    raise SystemExit

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit


class TestStream:
    def __init__(self, blocked):
        self.blocked = blocked

    def write(self, data):
        print("TestStream.write", data)
        if self.blocked:
            return None
        return len(data)


async def task(id, n, t):
    for i in range(n):
        print(id, i)
        await asyncio.sleep_ms(t)


async def main():
    t1 = asyncio.create_task(task(1, 4, 100))
    t2 = asyncio.create_task(task(2, 2, 250))

    # test scheduling tasks, and calling sleep_ms
    micropython.heap_lock()
    print("start")
    await asyncio.sleep_ms(5)
    print("sleep")
    await asyncio.sleep_ms(350)
    print("finish")
    micropython.heap_unlock()

    # test writing to a stream, when the underlying stream is blocked
    s = asyncio.StreamWriter(TestStream(True), None)
    micropython.heap_lock()
    s.write(b"12")
    micropython.heap_unlock()

    # test writing to a stream, when the underlying stream is not blocked
    buf = bytearray(b"56")
    s = asyncio.StreamWriter(TestStream(False), None)
    micropython.heap_lock()
    s.write(b"34")
    s.write(buf)
    micropython.heap_unlock()


asyncio.run(main())
