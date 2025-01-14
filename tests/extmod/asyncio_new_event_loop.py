# Test Loop.new_event_loop()

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit

# CPython 3.12 deprecated calling get_event_loop() when there is no current event
# loop, so to make this test run on CPython requires setting the event loop.
if hasattr(asyncio, "set_event_loop"):
    asyncio.set_event_loop(asyncio.new_event_loop())


def exception_handler(loop, context):
    # This is a workaround for a difference between CPython and MicroPython: if
    # a CPython event loop is closed while there are tasks pending (i.e. not finished)
    # on it, then the task will log an error. MicroPython does not log this error.
    if context.get("message", "") == "Task was destroyed but it is pending!":
        pass
    else:
        loop.default_exception_handler(context)


async def task():
    for i in range(4):
        print("task", i)
        await asyncio.sleep(0)
        await asyncio.sleep(0)


async def main():
    print("start")
    loop.create_task(task())
    await asyncio.sleep(0)  # yields, meaning new task will run once
    print("stop")
    loop.stop()


# Use default event loop to run some tasks
loop = asyncio.get_event_loop()
loop.set_exception_handler(exception_handler)
loop.create_task(main())
loop.run_forever()
loop.close()

# Create new event loop, old one should not keep running
loop = asyncio.new_event_loop()
loop.set_exception_handler(exception_handler)
loop.create_task(main())
loop.run_forever()
loop.close()
