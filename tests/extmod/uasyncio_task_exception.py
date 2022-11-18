# In MicroPython, a non-awaited task with a pending exception will raise to
# the loop's exception handler the second time it is scheduled. This is
# because without reference counting we have no way to know when the task is
# truly "non awaited" -- i.e. we only know that it wasn't awaited in the time
# it took to be re-scheduled.

# If the task _is_ subsequently awaited, then the await should succeed without
# raising.

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit


def custom_handler(loop, context):
    print("exception handler", type(context["exception"]).__name__)


async def main():
    loop = asyncio.get_event_loop()
    loop.set_exception_handler(custom_handler)

    async def task():
        print("raise")
        raise OSError

    print("create")
    t = asyncio.create_task(task())
    print("sleep 1")
    await asyncio.sleep(0)
    print("sleep 2")
    await asyncio.sleep(0)
    print("await")
    await t  # should not raise.


asyncio.run(main())
