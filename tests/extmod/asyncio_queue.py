# Test Queue class

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def producer(q, n):
    for i in range(n):
        await q.put(i)
        print("put", i)
    await q.join()
    print("producer done")


async def consumer(q, n):
    for _ in range(n):
        v = await q.get()
        print("got", v)
        q.task_done()


async def main():
    # Basic put_nowait/get_nowait, FIFO order.
    q = asyncio.Queue()
    print(q.qsize(), q.empty(), q.full())
    q.put_nowait(1)
    q.put_nowait(2)
    q.put_nowait(3)
    print(q.qsize(), q.empty(), q.full())
    print(q.get_nowait(), q.get_nowait(), q.get_nowait())
    print(q.qsize(), q.empty(), q.full())

    # get_nowait() on an empty queue raises QueueEmpty.
    try:
        q.get_nowait()
    except asyncio.QueueEmpty:
        print("QueueEmpty")

    # maxsize: full() and put_nowait() raising QueueFull.
    q = asyncio.Queue(maxsize=2)
    q.put_nowait(1)
    q.put_nowait(2)
    print(q.full())
    try:
        q.put_nowait(3)
    except asyncio.QueueFull:
        print("QueueFull")

    # put() blocks until get() frees a slot; get() blocks until an item exists.
    print("----")
    q = asyncio.Queue(maxsize=2)
    asyncio.create_task(producer(q, 5))
    await consumer(q, 5)
    await asyncio.sleep(0)

    # Multiple tasks waiting on get() are woken in FIFO order.
    print("----")
    q = asyncio.Queue()

    async def getter(i):
        v = await q.get()
        print("getter", i, "got", v)

    asyncio.create_task(getter(0))
    asyncio.create_task(getter(1))
    asyncio.create_task(getter(2))
    await asyncio.sleep(0)
    await q.put("a")
    await q.put("b")
    await q.put("c")
    await asyncio.sleep(0)

    # task_done() called more times than items retrieved raises ValueError.
    print("----")
    q = asyncio.Queue()
    q.put_nowait(1)
    q.get_nowait()
    q.task_done()
    try:
        q.task_done()
    except ValueError:
        print("ValueError")


asyncio.run(main())
