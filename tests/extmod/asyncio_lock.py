# Test Lock class
#
# This test has a corresponding .exp file because it tests a very specific ordering of
# events when cancelling a task waiting on a lock, and that ordering should not change
# (even though it does match CPython's output).

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def task_loop(id, lock):
    print("task start", id)
    for i in range(3):
        async with lock:
            print("task have", id, i)
    print("task end", id)


async def task_sleep(lock):
    async with lock:
        print("task have", lock.locked())
        await asyncio.sleep(0.2)
    print("task release", lock.locked())
    await lock.acquire()
    print("task have again")
    lock.release()


async def task_cancel(id, lock, to_cancel=None):
    try:
        async with lock:
            print("task got", id)
            await asyncio.sleep(0.1)
        print("task release", id)
        if to_cancel:
            to_cancel[0].cancel()
    except asyncio.CancelledError:
        print("task cancel", id)


async def main():
    lock = asyncio.Lock()

    # Basic acquire/release
    print(lock.locked())
    await lock.acquire()
    print(lock.locked())
    await asyncio.sleep(0)
    lock.release()
    print(lock.locked())
    await asyncio.sleep(0)

    # Use with "async with"
    async with lock:
        print("have lock")

    # 3 tasks wanting the lock
    print("----")
    asyncio.create_task(task_loop(1, lock))
    asyncio.create_task(task_loop(2, lock))
    t3 = asyncio.create_task(task_loop(3, lock))
    await lock.acquire()
    await asyncio.sleep(0)
    lock.release()
    await t3

    # 2 sleeping tasks both wanting the lock
    print("----")
    asyncio.create_task(task_sleep(lock))
    await asyncio.sleep(0.1)
    await task_sleep(lock)

    # 3 tasks, the first cancelling the second, the third should still run
    print("----")
    ts = [None]
    asyncio.create_task(task_cancel(0, lock, ts))
    ts[0] = asyncio.create_task(task_cancel(1, lock))
    asyncio.create_task(task_cancel(2, lock))
    await asyncio.sleep(0.3)
    print(lock.locked())

    # 3 tasks, the second and third being cancelled while waiting on the lock
    print("----")
    t0 = asyncio.create_task(task_cancel(0, lock))
    t1 = asyncio.create_task(task_cancel(1, lock))
    t2 = asyncio.create_task(task_cancel(2, lock))
    await asyncio.sleep(0.05)
    t1.cancel()
    await asyncio.sleep(0.1)
    t2.cancel()
    await asyncio.sleep(0.1)
    print(lock.locked())


asyncio.run(main())
