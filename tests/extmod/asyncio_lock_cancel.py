# Test that locks work when cancelling multiple waiters on the lock

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def task(i, lock, lock_flag):
    print("task", i, "start")
    try:
        await lock.acquire()
    except asyncio.CancelledError:
        print("task", i, "cancel")
        return
    print("task", i, "lock_flag", lock_flag[0])
    lock_flag[0] = True
    await asyncio.sleep(0)
    lock.release()
    lock_flag[0] = False
    print("task", i, "done")


async def main():
    # Create a lock and acquire it so the tasks below must wait
    lock = asyncio.Lock()
    await lock.acquire()
    lock_flag = [True]

    # Create 4 tasks and let them all run
    t0 = asyncio.create_task(task(0, lock, lock_flag))
    t1 = asyncio.create_task(task(1, lock, lock_flag))
    t2 = asyncio.create_task(task(2, lock, lock_flag))
    t3 = asyncio.create_task(task(3, lock, lock_flag))
    await asyncio.sleep(0)

    # Cancel 2 of the tasks (which are waiting on the lock) and release the lock
    t1.cancel()
    t2.cancel()
    lock.release()
    lock_flag[0] = False

    # Let the tasks run to completion
    for _ in range(4):
        await asyncio.sleep(0)

    # The locke should be unlocked
    print(lock.locked())


asyncio.run(main())
