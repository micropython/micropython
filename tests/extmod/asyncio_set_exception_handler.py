# Test that tasks return their value correctly to the caller

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


def custom_handler(loop, context):
    print("custom_handler", repr(context["exception"]))


async def task(i):
    # Raise with 2 args so exception prints the same in MicroPython and CPython
    raise ValueError(i, i + 1)


async def main():
    loop = asyncio.get_event_loop()

    # Check default exception handler, should be None
    print(loop.get_exception_handler())

    # Set exception handler and test it was set
    loop.set_exception_handler(custom_handler)
    print(loop.get_exception_handler() == custom_handler)

    # Create a task that raises and uses the custom exception handler
    asyncio.create_task(task(0))
    print("sleep")
    for _ in range(2):
        await asyncio.sleep(0)

    # Create 2 tasks to test order of printing exception
    asyncio.create_task(task(1))
    asyncio.create_task(task(2))
    print("sleep")
    for _ in range(2):
        await asyncio.sleep(0)

    # Create a task, let it run, then await it (no exception should be printed)
    t = asyncio.create_task(task(3))
    await asyncio.sleep(0)
    try:
        await t
    except ValueError as er:
        print(repr(er))

    print("done")


asyncio.run(main())
