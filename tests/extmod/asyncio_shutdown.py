try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit

import sys

# CPython and Micropython disagree on the sorting of tasks in the ::all_tasks
# set, so sort the output of the tasks as they shut down rather than expecting
# them to shut down in the same order.
class SortedList(list):
    def print(self):
        print(*sorted(self))

finished = SortedList()

async def task1():
    try:
        await asyncio.sleep(1)
    finally:
        finished.append('task1 finished')

async def task2():
    try:
        await asyncio.sleep(1)
    finally:
        finished.append("task2 finished")

async def reader():
    try:
        await asyncio.StreamReader(sys.stdin).read()
    finally:
        finished.append("reader finished")

# KeyboardInterrupt in main task should be propagated
async def ctrlc():
    asyncio.create_task(task1())
    await asyncio.sleep(0)
    asyncio.create_task(task2())
    await asyncio.sleep(0.01)
    raise KeyboardInterrupt


# If the main task is canceled, the loop should be closed
async def cancel_main():
    async def canceler(delay, task):
        await asyncio.sleep(delay)
        task.cancel()

    asyncio.create_task(task1())
    await asyncio.sleep(0)
    asyncio.create_task(task2())
    asyncio.create_task(canceler(0.01, asyncio.current_task()))
    await asyncio.sleep(0.02)

# SystemExit should close the loop
async def sys_exit():
    asyncio.create_task(task1())
    await asyncio.sleep(0)
    asyncio.create_task(task2())
    await asyncio.sleep(0.01)
    raise SystemExit

# Cancelling a background task should have no effect
async def cancel_bg():
    t1 = asyncio.create_task(task1())
    await asyncio.sleep(0)
    t2 = asyncio.create_task(task2())
    await asyncio.sleep(0.01)
    t1.cancel()
    t2.cancel()
    await asyncio.sleep(0.01)

# Reader tasks should also be cancelled when the loop is shutdown
async def cancel_reader():
    t1 = asyncio.create_task(task1())
    await asyncio.sleep(0)
    t2 = asyncio.create_task(reader())
    await asyncio.sleep(0.01)
    t1.cancel()
    t2.cancel()
    await asyncio.sleep(0.01)

async def close_loop():
    asyncio.create_task(task1())
    await asyncio.sleep(0)
    asyncio.create_task(reader())
    await asyncio.sleep(0.01)

try:
    asyncio.new_event_loop()
    finished.clear()
    asyncio.run(ctrlc())
except KeyboardInterrupt:
    print("KeyboardInterrupt raised")
finally:
    finished.print()

try:
    asyncio.new_event_loop()
    finished.clear()
    asyncio.run(cancel_main())
except asyncio.CancelledError:
    print("CancelledError raised")
finally:
    finished.print()

# Cancel a background waiting task doesn't raise an exception
print('---')
asyncio.new_event_loop()
finished.clear()
asyncio.run(cancel_bg())
finished.print()

# Cancel a background IO task doesn't raise an exception
print('---')
asyncio.new_event_loop()
finished.clear()
asyncio.run(cancel_reader())
finished.print()

print('---')
asyncio.new_event_loop()
finished.clear()
asyncio.run(cancel_reader())
finished.print()

try:
    asyncio.new_event_loop()
    finished.clear()
    asyncio.run(sys_exit())
except SystemExit:
    print("SystemExit raised")

