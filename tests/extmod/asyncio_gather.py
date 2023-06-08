# test asyncio.gather() function

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


async def task(id, t=0.1):
    print("start", id)
    await asyncio.sleep(t)
    print("end", id)
    return id


async def task_loop(id):
    print("task_loop start", id)
    while True:
        await asyncio.sleep(0.1)
        print("task_loop loop", id)


async def task_raise(id, t=0.1):
    print("task_raise start", id)
    await asyncio.sleep(t)
    print("task_raise raise", id)
    raise ValueError(id)


async def gather_task(t0, t1):
    print("gather_task")
    await asyncio.gather(t0, t1)
    print("gather_task2")


async def main():
    # Simple gather with return values
    print(await asyncio.gather(factorial("A", 2), factorial("B", 3), factorial("C", 4)))

    print("====")

    # Gather with no awaitables
    print(await asyncio.gather())

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
    await asyncio.sleep(0.2)

    print("====")

    # Test case where both tasks raise an exception.
    # Use t=0 so they raise one after the other, between the gather starting and finishing.
    tasks = [asyncio.create_task(task_raise(1, t=0)), asyncio.create_task(task_raise(2, t=0))]
    try:
        await asyncio.gather(*tasks)
    except ValueError as er:
        print(repr(er))
    print(tasks[0].done(), tasks[1].done())

    print("====")

    # Cancel a multi gather.
    t = asyncio.create_task(gather_task(task(1), task(2)))
    await asyncio.sleep(0.05)
    t.cancel()
    await asyncio.sleep(0.2)

    # Test edge cases where the gather is cancelled just as tasks are created and ending.
    for i in range(1, 4):
        print("====")
        t = asyncio.create_task(gather_task(task(1, t=0), task(2, t=0)))
        for _ in range(i):
            await asyncio.sleep(0)
        t.cancel()
        await asyncio.sleep(0.2)


asyncio.run(main())
