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


async def task_loop(id):
    print("task_loop start", id)
    while True:
        await asyncio.sleep(0.02)
        print("task_loop loop", id)


async def task_raise(id):
    print("task_raise start", id)
    await asyncio.sleep(0.02)
    raise ValueError(id)


async def gather_task(t0, t1):
    print("gather_task")
    await asyncio.gather(t0, t1)
    print("gather_task2")


async def main():
    # Simple gather with return values
    print(await asyncio.gather(factorial("A", 2), factorial("B", 3), factorial("C", 4)))

    print("====")

    # Test return_exceptions, where one task is cancelled and the other finishes normally
    tasks = [asyncio.create_task(task(1)), asyncio.create_task(task(2))]
    tasks[0].cancel()
    print(await asyncio.gather(*tasks, return_exceptions=True))

    print("====")

    # Test return_exceptions, where one task raises an exception and the other finishes normally.
    tasks = [asyncio.create_task(task(1)), asyncio.create_task(task_raise(2))]
    print(await asyncio.gather(*tasks, return_exceptions=True))

    print("====")

    # Test case where one task raises an exception and other task keeps running.
    tasks = [asyncio.create_task(task_loop(1)), asyncio.create_task(task_raise(2))]
    try:
        await asyncio.gather(*tasks)
    except ValueError as er:
        print(repr(er))
    print(tasks[0].done(), tasks[1].done())
    for t in tasks:
        t.cancel()
    await asyncio.sleep(0.04)

    print("====")

    # Test case where both tasks raise an exception.
    tasks = [asyncio.create_task(task_raise(1)), asyncio.create_task(task_raise(2))]
    try:
        await asyncio.gather(*tasks)
    except ValueError as er:
        print(repr(er))
    print(tasks[0].done(), tasks[1].done())

    print("====")

    # Cancel a multi gather.
    t = asyncio.create_task(gather_task(task(1), task(2)))
    await asyncio.sleep(0.01)
    t.cancel()
    await asyncio.sleep(0.04)


asyncio.run(main())
