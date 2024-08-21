# Test that tasks return their value correctly to the caller

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def foo():
    return 42


# CIRCUITPY-CHANGE: await
try:
    foo().__await__
except AttributeError:
    print("SKIP")
    raise SystemExit


async def main():
    # Call function directly via an await
    print(await foo())

    # Create a task and await on it
    task = asyncio.create_task(foo())
    print(await task)


asyncio.run(main())
