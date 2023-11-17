# Test the `Task.cancelled` method

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def task(t):
    await asyncio.sleep(t)


async def main():
    # Cancel task immediately doesn't mark the task as cancelled
    print("=" * 10)
    t = asyncio.create_task(task(2))
    t.cancel()
    print("Expecting task to not be cancelled because it is not done:", t.cancelled())

    # Cancel task immediately and wait for cancellation to complete
    print("=" * 10)
    t = asyncio.create_task(task(2))
    t.cancel()
    await asyncio.sleep(0)
    print("Expecting Task to be Cancelled:", t.cancelled())

    # Cancel task and wait for cancellation to complete
    print("=" * 10)
    t = asyncio.create_task(task(2))
    await asyncio.sleep(0.01)
    t.cancel()
    await asyncio.sleep(0)
    print("Expecting Task to be Cancelled:", t.cancelled())

    # Cancel task multiple times after it has started
    print("=" * 10)
    t = asyncio.create_task(task(2))
    await asyncio.sleep(0.01)
    for _ in range(4):
        t.cancel()
    await asyncio.sleep(0.01)

    print("Expecting Task to be Cancelled:", t.cancelled())

    # Cancel task after it has finished
    print("=" * 10)
    t = asyncio.create_task(task(0.01))
    await asyncio.sleep(0.05)
    t.cancel()
    print("Expecting task to not be Cancelled:", t.cancelled())


asyncio.run(main())
