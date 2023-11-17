# Test the `Task.get_coro()` method

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def action():
    pass


async def main():
    # Check that the coro we include is the same coro we get back
    print("=" * 10)

    coro = action()
    t = asyncio.create_task(coro)
    print(t.get_coro() == coro)

    # Check that the coro prop matches the get_coro() result
    print("=" * 10)
    t = asyncio.create_task(action())
    print(t.get_coro() == t.coro)


asyncio.run(main())
