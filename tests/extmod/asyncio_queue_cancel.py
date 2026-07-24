# Test that Queue works when cancelling tasks waiting on get(), put() and join()

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def getter(i, q):
    print("getter", i, "start")
    try:
        v = await q.get()
    except asyncio.CancelledError:
        print("getter", i, "cancel")
        return
    print("getter", i, "got", v)


async def putter(i, q):
    print("putter", i, "start")
    try:
        await q.put(i)
    except asyncio.CancelledError:
        print("putter", i, "cancel")
        return
    print("putter", i, "done")


async def joiner(i, q):
    print("joiner", i, "start")
    try:
        await q.join()
    except asyncio.CancelledError:
        print("joiner", i, "cancel")
        return
    print("joiner", i, "done")


async def main():
    # Cancel some of several tasks waiting on an empty queue's get(); the
    # rest should be unaffected and still receive an item.
    q = asyncio.Queue()
    t0 = asyncio.create_task(getter(0, q))
    t1 = asyncio.create_task(getter(1, q))
    t2 = asyncio.create_task(getter(2, q))
    await asyncio.sleep(0)
    t1.cancel()
    await asyncio.sleep(0)
    await q.put("x")
    await q.put("y")
    await asyncio.sleep(0)

    # Cancel some of several tasks waiting on a full queue's put(); the rest
    # should be unaffected and still get their item queued once space frees.
    print("----")
    q = asyncio.Queue(maxsize=1)
    q.put_nowait("full")
    t0 = asyncio.create_task(putter(0, q))
    t1 = asyncio.create_task(putter(1, q))
    t2 = asyncio.create_task(putter(2, q))
    await asyncio.sleep(0)
    t1.cancel()
    await asyncio.sleep(0)
    q.get_nowait()  # frees a slot for one waiting putter
    await asyncio.sleep(0)
    q.get_nowait()  # frees a slot for the other waiting putter
    await asyncio.sleep(0)

    # Cancel a task waiting on join(); a task_done() completing all
    # outstanding items should still wake any remaining joiner.
    print("----")
    q = asyncio.Queue()
    q.put_nowait(1)
    q.put_nowait(2)
    t0 = asyncio.create_task(joiner(0, q))
    t1 = asyncio.create_task(joiner(1, q))
    await asyncio.sleep(0)
    t0.cancel()
    await asyncio.sleep(0)
    q.get_nowait()
    q.task_done()
    q.get_nowait()
    q.task_done()
    await asyncio.sleep(0)

    # The queue should still be fully usable after all that cancellation.
    print("----")
    q = asyncio.Queue()
    q.put_nowait("still works")
    print(await q.get())


asyncio.run(main())
