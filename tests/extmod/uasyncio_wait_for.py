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


async def task_cancel_other(t, other):
    print("task_cancel_other start")
    await asyncio.sleep(t)
    print("task_cancel_other cancel")
    other.cancel()


async def task_wait_for_cancel(id, t, t_wait):
    print("task_wait_for_cancel start")
    try:
        await asyncio.wait_for(task(id, t), t_wait)
    except asyncio.CancelledError as er:
        print("task_wait_for_cancel cancelled")
        raise er


async def task_wait_for_cancel_ignore(t_wait):
    print("task_wait_for_cancel_ignore start")
    try:
        await asyncio.wait_for(task_catch(), t_wait)
    except asyncio.CancelledError as er:
        print("task_wait_for_cancel_ignore cancelled")
        raise er


async def main():
    sep = "-" * 10

    # When task finished before the timeout
    print(await asyncio.wait_for(task(1, 0.01), 10))
    print(sep)

    # When timeout passes and task is cancelled
    try:
        print(await asyncio.wait_for(task(2, 10), 0.01))
    except asyncio.TimeoutError:
        print("timeout")
    print(sep)

    # When timeout passes and task is cancelled, but task ignores the cancellation request
    try:
        print(await asyncio.wait_for(task_catch(), 0.1))
    except asyncio.TimeoutError:
        print("TimeoutError")
    print(sep)

    # When task raises an exception
    try:
        print(await asyncio.wait_for(task_raise(), 1))
    except ValueError:
        print("ValueError")
    print(sep)

    # Timeout of None means wait forever
    print(await asyncio.wait_for(task(3, 0.1), None))
    print(sep)

    # When task is cancelled by another task
    t = asyncio.create_task(task(4, 10))
    asyncio.create_task(task_cancel_other(0.01, t))
    try:
        print(await asyncio.wait_for(t, 1))
    except asyncio.CancelledError as er:
        print(repr(er))
    print(sep)

    # When wait_for gets cancelled
    t = asyncio.create_task(task_wait_for_cancel(4, 1, 2))
    await asyncio.sleep(0.01)
    t.cancel()
    await asyncio.sleep(0.01)
    print(sep)

    # When wait_for gets cancelled and awaited task ignores the cancellation request
    t = asyncio.create_task(task_wait_for_cancel_ignore(2))
    await asyncio.sleep(0.01)
    t.cancel()
    await asyncio.sleep(0.01)
    print(sep)

    # When wait_for gets cancelled and the task it's waiting on finishes around the
    # same time as the cancellation of the wait_for
    for num_sleep in range(1, 5):
        t = asyncio.create_task(task_wait_for_cancel(4 + num_sleep, 0, 2))
        for _ in range(num_sleep):
            await asyncio.sleep(0)
        assert not t.done()
        print("cancel wait_for")
        t.cancel()
        try:
            await t
        except asyncio.CancelledError as er:
            print(repr(er))
        print(sep)

    print("finish")


asyncio.run(main())
