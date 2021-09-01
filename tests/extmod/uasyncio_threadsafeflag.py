# Test Event class

try:
    import uasyncio as asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


import micropython

try:
    micropython.schedule
except AttributeError:
    print("SKIP")
    raise SystemExit


try:
    # Unix port can't select/poll on user-defined types.
    import uselect as select

    poller = select.poll()
    poller.register(asyncio.ThreadSafeFlag())
except TypeError:
    print("SKIP")
    raise SystemExit


async def task(id, flag):
    print("task", id)
    await flag.wait()
    print("task", id, "done")


def set_from_schedule(flag):
    print("schedule")
    flag.set()
    print("schedule done")


async def main():
    flag = asyncio.ThreadSafeFlag()

    # Set the flag from within the loop.
    t = asyncio.create_task(task(1, flag))
    print("yield")
    await asyncio.sleep(0)
    print("set event")
    flag.set()
    print("yield")
    await asyncio.sleep(0)
    print("wait task")
    await t

    # Set the flag from scheduler context.
    print("----")
    t = asyncio.create_task(task(2, flag))
    print("yield")
    await asyncio.sleep(0)
    print("set event")
    micropython.schedule(set_from_schedule, flag)
    print("yield")
    await asyncio.sleep(0)
    print("wait task")
    await t

    # Flag already set.
    print("----")
    print("set event")
    flag.set()
    t = asyncio.create_task(task(3, flag))
    print("yield")
    await asyncio.sleep(0)
    print("wait task")
    await t

    # Flag set, cleared, and set again.
    print("----")
    print("set event")
    flag.set()
    print("yield")
    await asyncio.sleep(0)
    print("clear event")
    flag.clear()
    print("yield")
    await asyncio.sleep(0)
    t = asyncio.create_task(task(4, flag))
    print("yield")
    await asyncio.sleep(0)
    print("set event")
    flag.set()
    print("yield")
    await asyncio.sleep(0)
    print("wait task")
    await t


asyncio.run(main())
