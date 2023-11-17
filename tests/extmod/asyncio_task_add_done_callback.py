# Test the Task.add_done_callback() method

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


def done_callback(t, er):
    print("done", repr(t), repr(er))


async def main():
    # Tasks that aren't done only execute done callback after finishing
    print("=" * 10)
    t = asyncio.create_task(task(-1))
    t.add_done_callback(done_callback)
    print("Waiting for task to complete")
    await asyncio.sleep(0)
    print("Task has completed")

    # Task that are done run the callback immediately
    print("=" * 10)
    t = asyncio.create_task(task(-1))
    await asyncio.sleep(0)
    print("Task has completed")
    t.add_done_callback(done_callback)
    print("Callback Added")

    # Task that starts, runs and finishes without an exception should return None
    print("=" * 10)
    t = asyncio.create_task(task(0.01))
    t.add_done_callback(done_callback)
    try:
        t.add_done_callback(done_callback)
    except RuntimeError as e:
        print("Second call to add_done_callback emits error:", repr(e))

    # Task that raises immediately should still run done callback
    print("=" * 10)
    t = asyncio.create_task(task(-1, ValueError))
    t.add_done_callback(done_callback)
    await asyncio.sleep(0)


asyncio.run(main())
