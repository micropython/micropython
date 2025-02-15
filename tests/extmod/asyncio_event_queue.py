# Ensure that an asyncio task can wait on an Event when the
# _task_queue is empty
# https://github.com/micropython/micropython/issues/16569

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit

# This test requires to check that the asyncio scheduler
# stays active "indefinitely" when the task queue is empty.
#
# To verify this, we need another independent scheduler that
# can wait for a specified amount of time. So we need to
# create one using micropython.schedule() and time.ticks_ms()
#
# This will also ensure that in real life the task can be
# waken from a schedule(), eg. from an IRQ handler

import micropython, time

try:
    micropython.schedule
except AttributeError:
    print("SKIP")
    raise SystemExit


evt = asyncio.Event()


def schedule_watchdog(end_ticks):
    if time.ticks_diff(end_ticks, time.ticks_ms()) <= 0:
        print("asyncio still pending, unlocking event")
        evt.set()
        return
    micropython.schedule(schedule_watchdog, end_ticks)


async def foo():
    print("foo waiting")
    schedule_watchdog(time.ticks_add(time.ticks_ms(), 100))
    await evt.wait()
    print("foo done")


async def main():
    print("main started")
    await foo()
    print("main done")


asyncio.run(main())
