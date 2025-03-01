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


async def task():
    for i in range(4):
        print("task", i)
        await asyncio.sleep(0)
        await asyncio.sleep(0)


async def main():
    print("start")
    loop.create_task(task())
    await asyncio.sleep(0)
    print("stop")
    loop.stop()


# Use default event loop to run some tasks
loop = asyncio.get_event_loop()
loop.create_task(main())
loop.run_forever()

# Create new event loop, old one should not keep running
loop = asyncio.new_event_loop()
loop.create_task(main())
loop.run_forever()
