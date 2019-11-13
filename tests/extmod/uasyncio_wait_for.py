# Test asyncio.wait_for

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit


async def task(id, t):
    print("task start", id)
    await asyncio.sleep(t)
    print("task end", id)
    return id * 2


async def task_catch():
    print("task_catch start")
    try:
        await asyncio.sleep(0.2)
    except asyncio.CancelledError:
        print("ignore cancel")
    print("task_catch done")


async def task_raise():
    print("task start")
    raise ValueError


async def main():
    # When task finished before the timeout
    print(await asyncio.wait_for(task(1, 0.01), 10))

    # When timeout passes and task is cancelled
    try:
        print(await asyncio.wait_for(task(2, 10), 0.01))
    except asyncio.TimeoutError:
        print("timeout")

    # When timeout passes and task is cancelled, but task ignores the cancellation request
    try:
        print(await asyncio.wait_for(task_catch(), 0.1))
    except asyncio.TimeoutError:
        print("TimeoutError")

    # When task raises an exception
    try:
        print(await asyncio.wait_for(task_raise(), 1))
    except ValueError:
        print("ValueError")

    # Timeout of None means wait forever
    print(await asyncio.wait_for(task(3, 0.1), None))

    print("finish")


asyncio.run(main())
