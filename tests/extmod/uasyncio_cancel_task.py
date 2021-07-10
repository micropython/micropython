# Test cancelling a task

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit


async def task(s, allow_cancel):
    try:
        print("task start")
        await asyncio.sleep(s)
        print("task done")
    except asyncio.CancelledError as er:
        print("task cancel")
        if allow_cancel:
            raise er


async def task2(allow_cancel):
    print("task 2")
    try:
        await asyncio.create_task(task(0.05, allow_cancel))
    except asyncio.CancelledError as er:
        print("task 2 cancel")
        raise er
    print("task 2 done")


async def main():
    # Cancel task immediately
    t = asyncio.create_task(task(2, True))
    print(t.cancel())

    # Cancel task after it has started
    t = asyncio.create_task(task(2, True))
    await asyncio.sleep(0.01)
    print(t.cancel())
    print("main sleep")
    await asyncio.sleep(0.01)

    # Cancel task multiple times after it has started
    t = asyncio.create_task(task(2, True))
    await asyncio.sleep(0.01)
    for _ in range(4):
        print(t.cancel())
    print("main sleep")
    await asyncio.sleep(0.01)

    # Await on a cancelled task
    print("main wait")
    try:
        await t
    except asyncio.CancelledError:
        print("main got CancelledError")

    # Cancel task after it has finished
    t = asyncio.create_task(task(0.01, False))
    await asyncio.sleep(0.05)
    print(t.cancel())

    # Nested: task2 waits on task, task2 is cancelled (should cancel task then task2)
    print("----")
    t = asyncio.create_task(task2(True))
    await asyncio.sleep(0.01)
    print("main cancel")
    t.cancel()
    print("main sleep")
    await asyncio.sleep(0.1)

    # Nested: task2 waits on task, task2 is cancelled but task doesn't allow it (task2 should continue)
    print("----")
    t = asyncio.create_task(task2(False))
    await asyncio.sleep(0.01)
    print("main cancel")
    t.cancel()
    print("main sleep")
    await asyncio.sleep(0.1)


asyncio.run(main())
