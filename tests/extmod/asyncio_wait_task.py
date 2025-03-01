# Test waiting on a task

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


import time

if hasattr(time, "ticks_ms"):
    ticks = time.ticks_ms
    ticks_diff = time.ticks_diff
else:
    ticks = lambda: int(time.time() * 1000)
    ticks_diff = lambda t1, t0: t1 - t0


async def task(t):
    print("task", t)


async def delay_print(t, s):
    await asyncio.sleep(t)
    print(s)


async def task_raise():
    print("task_raise")
    raise ValueError


async def main():
    print("start")

    # Wait on a task
    t = asyncio.create_task(task(1))
    await t

    # Wait on a task that's already done
    t = asyncio.create_task(task(2))
    await asyncio.sleep(0.001)
    await t

    # Wait again on same task
    await t

    print("----")

    # Create 2 tasks
    ts1 = asyncio.create_task(delay_print(0.2, "hello"))
    ts2 = asyncio.create_task(delay_print(0.4, "world"))

    # Time how long the tasks take to finish, they should execute in parallel
    print("start")
    t0 = ticks()
    await ts1
    t1 = ticks()
    await ts2
    t2 = ticks()
    print("took {} {}".format(round(ticks_diff(t1, t0), -2), round(ticks_diff(t2, t1), -2)))

    # Wait on a task that raises an exception
    t = asyncio.create_task(task_raise())
    try:
        await t
    except ValueError:
        print("ValueError")

    # Wait on a task that raises, but the waiting is done some time later.
    # Need to suppress the "Task exception wasn't retrieved" message.
    asyncio.get_event_loop().set_exception_handler(lambda loop, context: None)
    t = asyncio.create_task(task_raise())
    for _ in range(5):
        await asyncio.sleep(0)
    try:
        await t
    except ValueError:
        print("ValueError")


asyncio.run(main())
