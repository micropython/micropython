try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit

import sys

async def waiter():
    await asyncio.sleep(1)

async def reader():
    await asyncio.StreamReader().read()


async def print_all_tasks():
    print(list('T' for t in asyncio.all_tasks()))

async def short_wait():
    await asyncio.sleep(0.01)

async def waiters():
    t1 = asyncio.create_task(waiter())
    # NOTE: The singleton generator for ::sleep expects the task to be scheduled
    # before using it again.
    await asyncio.sleep(0)
    t2 = asyncio.create_task(waiter())
    await asyncio.sleep(0)
    t3 = asyncio.create_task(short_wait())
    # Tasks are in the list before they start to run
    assert t1 in asyncio.all_tasks()
    assert t2 in asyncio.all_tasks()
    await print_all_tasks()
    await t3
    assert t1 in asyncio.all_tasks()
    assert t2 in asyncio.all_tasks()
    assert t3 not in asyncio.all_tasks()

async def readers():
    t1 = asyncio.create_task(reader())
    t2 = asyncio.create_task(reader())
    await print_all_tasks()
    assert t1 in asyncio.all_tasks()
    assert t2 in asyncio.all_tasks()

asyncio.run(print_all_tasks())
asyncio.new_event_loop()
asyncio.run(readers())
asyncio.new_event_loop()
asyncio.run(waiters())