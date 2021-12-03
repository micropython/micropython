# test uasyncio.gather() function

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit


async def factorial(name, number):
    f = 1
    for i in range(2, number + 1):
        print("Task {}: Compute factorial({})...".format(name, i))
        await asyncio.sleep(0.01)
        f *= i
    print("Task {}: factorial({}) = {}".format(name, number, f))
    return f


async def task(id):
    print("start", id)
    await asyncio.sleep(0.02)
    print("end", id)
    return id


async def gather_task():
    print("gather_task")
    await asyncio.gather(task(1), task(2))
    print("gather_task2")


async def main():
    # Simple gather with return values
    print(await asyncio.gather(factorial("A", 2), factorial("B", 3), factorial("C", 4)))

    # Test return_exceptions, where one task is cancelled and the other finishes normally
    tasks = [asyncio.create_task(task(1)), asyncio.create_task(task(2))]
    tasks[0].cancel()
    print(await asyncio.gather(*tasks, return_exceptions=True))

    # Cancel a multi gather
    # TODO doesn't work, Task should not forward cancellation from gather to sub-task
    # but rather CancelledError should cancel the gather directly, which will then cancel
    # all sub-tasks explicitly
    # t = asyncio.create_task(gather_task())
    # await asyncio.sleep(0.01)
    # t.cancel()
    # await asyncio.sleep(0.01)


asyncio.run(main())
