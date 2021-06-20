# Test Event class

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit


async def task(id, ev):
    print("start", id)
    print(await ev.wait())
    print("end", id)


async def task_delay_set(t, ev):
    await asyncio.sleep(t)
    print("set event")
    ev.set()


async def main():
    ev = asyncio.Event()

    # Set and clear without anything waiting, and test is_set()
    print(ev.is_set())
    ev.set()
    print(ev.is_set())
    ev.clear()
    print(ev.is_set())

    # Create 2 tasks waiting on the event
    print("----")
    asyncio.create_task(task(1, ev))
    asyncio.create_task(task(2, ev))
    print("yield")
    await asyncio.sleep(0)
    print("set event")
    ev.set()
    print("yield")
    await asyncio.sleep(0)

    # Create a task waiting on the already-set event
    print("----")
    asyncio.create_task(task(3, ev))
    print("yield")
    await asyncio.sleep(0)

    # Clear event, start a task, then set event again
    print("----")
    print("clear event")
    ev.clear()
    asyncio.create_task(task(4, ev))
    await asyncio.sleep(0)
    print("set event")
    ev.set()
    await asyncio.sleep(0)

    # Cancel a task waiting on an event (set event then cancel task)
    print("----")
    ev = asyncio.Event()
    t = asyncio.create_task(task(5, ev))
    await asyncio.sleep(0)
    ev.set()
    t.cancel()
    await asyncio.sleep(0.1)

    # Cancel a task waiting on an event (cancel task then set event)
    print("----")
    ev = asyncio.Event()
    t = asyncio.create_task(task(6, ev))
    await asyncio.sleep(0)
    t.cancel()
    ev.set()
    await asyncio.sleep(0.1)

    # Wait for an event that does get set in time
    print("----")
    ev.clear()
    asyncio.create_task(task_delay_set(0.01, ev))
    await asyncio.wait_for(ev.wait(), 0.1)
    await asyncio.sleep(0)

    # Wait for an event that doesn't get set in time
    print("----")
    ev.clear()
    asyncio.create_task(task_delay_set(0.1, ev))
    try:
        await asyncio.wait_for(ev.wait(), 0.01)
    except asyncio.TimeoutError:
        print("TimeoutError")
    await ev.wait()


asyncio.run(main())
