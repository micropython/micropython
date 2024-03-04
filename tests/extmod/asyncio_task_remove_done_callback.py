# Test the Task.remove_done_callback() method

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def task(t, exc=None):
    if t >= 0:
        await asyncio.sleep(t)
    if exc:
        raise exc


def done_callback():
    print("done")


def done_callback_2():
    print("done 2")


async def main():
    # Removing a callback returns 0 when no callbacks have been set
    print("=" * 10)
    t = asyncio.create_task(task(1))
    print("Returns 0 when no done callback has been set:", t.remove_done_callback(done_callback))

    # Done callback removal only works once
    print("=" * 10)
    t = asyncio.create_task(task(1))
    t.add_done_callback(done_callback)
    print(
        "Returns 1 when a callback matches and is removed:", t.remove_done_callback(done_callback)
    )
    print(
        "Returns 0 on second attempt to remove the callback:",
        t.remove_done_callback(done_callback),
    )

    # Only removes done callback when match
    print("=" * 10)
    t = asyncio.create_task(task(0.01))
    t.add_done_callback(done_callback)
    print("Returns 0 when done callbacks don't match:", t.remove_done_callback(done_callback_2))

    # A removed done callback does not execute
    print("=" * 10)
    t = asyncio.create_task(task(-1))
    t.add_done_callback(done_callback)
    t.remove_done_callback(done_callback)
    print("Waiting for task to complete")
    await t
    print("Task completed")


asyncio.run(main())
