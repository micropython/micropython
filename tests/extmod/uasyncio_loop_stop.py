# Test Loop.stop() to stop the event loop

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit


async def task():
    print("task")


async def main():
    print("start")

    # Stop the loop after next yield
    loop.stop()

    # Check that calling stop() again doesn't do/break anything
    loop.stop()

    # This await should stop
    print("sleep")
    await asyncio.sleep(0)

    # Schedule stop, then create a new task, then yield
    loop.stop()
    asyncio.create_task(task())
    await asyncio.sleep(0)

    # Final stop
    print("end")
    loop.stop()


loop = asyncio.get_event_loop()
loop.create_task(main())

for i in range(3):
    print("run", i)
    loop.run_forever()
