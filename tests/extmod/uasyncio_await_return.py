# Test that tasks return their value correctly to the caller

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit


async def foo():
    return 42


async def main():
    # Call function directly via an await
    print(await foo())

    # Create a task and await on it
    task = asyncio.create_task(foo())
    print(await task)


asyncio.run(main())
