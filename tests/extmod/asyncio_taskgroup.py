# test asyncio.TaskGroup functions

# Origin: a straightforward translation of the uasyncio.gather test script.

import sys

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
        try:
            await asyncio.sleep(0.1)
        except asyncio.CancelledError:
            print("task_loop cancelled", id)
            raise
        print("task_loop loop", id)


async def task_loop_nc(id):
    print("task_loop_nc start", id)
    for _ in range(id):
        try:
            await asyncio.sleep(0.1)
        except asyncio.CancelledError:
            print("task_loop_nc persists", id)
            continue
        print("task_loop_nc loop", id)


async def task_raise(id, t=0.1):
    print("task_raise start", id)
    try:
        await asyncio.sleep(t)
    except asyncio.CancelledError:
        pass
    print("task_raise raise", id)
    raise ValueError(id)


def check_done(tasks):
    for t in tasks:
        if not t.done():
            print("FAIL running", t)


async def main():
    # Simple gather with return values
    async with asyncio.TaskGroup() as tg:
        tasks = [
            tg.create_task(factorial("A", 2)),
            tg.create_task(factorial("B", 3)),
            tg.create_task(factorial("C", 4)),
        ]
    check_done(tasks)

    try:
        # Test cancellation, where one task is cancelled and the other finishes normally
        await asyncio.sleep(0.1)
        async with asyncio.TaskGroup() as tg:
            await asyncio.sleep(0.1)
            tasks = [tg.create_task(task(1)), tg.create_task(task(2))]
            tasks[0].cancel()
        check_done(tasks)
        print("====")
    except BaseException as ex:
        print("FAIL", repr(ex))

    try:
        async with asyncio.TaskGroup() as tg:
            tasks = [tg.create_task(task(1)), tg.create_task(task_raise(2))]
    except ValueError as er:
        print("OK", repr(er))
    else:
        print("FAIL no error raised")
    check_done(tasks)

    print("====")

    # Test case where one task raises an exception and other task keeps running.
    try:
        async with asyncio.TaskGroup() as tg:
            tasks = [tg.create_task(task_loop_nc(3)), tg.create_task(task_raise(2))]
    except ValueError as er:
        print("OK", repr(er))
    else:
        print("FAIL passed")
    check_done(tasks)

    print("====")

    # Test case where both tasks raise an exception.
    # Use t=0 so they raise one after the other, between the gather starting and finishing.
    try:
        async with asyncio.TaskGroup() as tg:
            tasks = [tg.create_task(task_raise(1, t=0)), tg.create_task(task_raise(2, t=0))]
    except asyncio.ExceptionGroup as er:
        print("OK", repr(er))
    except ValueError as er:
        # we'd hit this case if "task_raise" wouldn't ignore cancellations
        print("FAIL", repr(er))
    else:
        print("FAIL passed")
    check_done(tasks)

    print("====")

    # Cancel a taskgroup.
    async with asyncio.TaskGroup() as tg:
        tasks = [tg.create_task(task(1)), tg.create_task(task(2))]
        await asyncio.sleep(0.05)
        tg.cancel()
    check_done(tasks)

    # Test edge cases where the gather is cancelled just as tasks are created and ending.
    for i in range(1, 4):
        print("====")
        async with asyncio.TaskGroup() as tg:
            tasks = [tg.create_task(task(1, t=0)), tg.create_task(task(2, t=0))]
            for _ in range(i):
                await asyncio.sleep(0)
            tg.cancel()
        check_done(tasks)
    print("====")

    # Test that the system still works and doesn't save any cancellations
    await asyncio.sleep(0.1)
    print("====")


asyncio.run(main())
