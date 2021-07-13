# Test the Task.done() method

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit


async def task(t, exc=None):
    print("task start")
    if t >= 0:
        await asyncio.sleep(t)
    if exc:
        raise exc
    print("task done")


async def main():
    # Task that finishes immediately.
    print("=" * 10)
    t = asyncio.create_task(task(-1))
    print(t.done())
    await asyncio.sleep(0)
    print(t.done())
    await t
    print(t.done())

    # Task that starts, runs and finishes.
    print("=" * 10)
    t = asyncio.create_task(task(0.01))
    print(t.done())
    await asyncio.sleep(0)
    print(t.done())
    await t
    print(t.done())

    # Task that raises immediately.
    print("=" * 10)
    t = asyncio.create_task(task(-1, ValueError))
    print(t.done())
    await asyncio.sleep(0)
    print(t.done())
    try:
        await t
    except ValueError as er:
        print(repr(er))
    print(t.done())

    # Task that raises after a delay.
    print("=" * 10)
    t = asyncio.create_task(task(0.01, ValueError))
    print(t.done())
    await asyncio.sleep(0)
    print(t.done())
    try:
        await t
    except ValueError as er:
        print(repr(er))
    print(t.done())


asyncio.run(main())
