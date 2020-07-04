# Test fairness of Event.set()
# That tasks which continuously wait on events don't take over the scheduler

try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit


async def task1(id):
    for i in range(4):
        print("sleep", id)
        await asyncio.sleep(0)


async def task2(id, ev):
    for i in range(4):
        ev.set()
        ev.clear()
        print("wait", id)
        await ev.wait()


async def main():
    ev = asyncio.Event()
    tasks = [
        asyncio.create_task(task1(0)),
        asyncio.create_task(task2(2, ev)),
        asyncio.create_task(task1(1)),
        asyncio.create_task(task2(3, ev)),
    ]
    await tasks[1]
    ev.set()


asyncio.run(main())
