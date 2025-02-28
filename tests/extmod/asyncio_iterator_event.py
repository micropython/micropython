# Ensure that an asyncio task can wait on an Event when the
# _task_queue is empty, in the context of an async iterator
# https://github.com/micropython/micropython/issues/16318

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

import micropython, time

try:
    micropython.schedule
except AttributeError:
    print("SKIP")
    raise SystemExit

ai = None


def schedule_watchdog(end_ticks):
    if time.ticks_diff(end_ticks, time.ticks_ms()) <= 0:
        print("good: asyncio iterator is still pending, exiting")
        ai.fetch_data(None)
        return
    micropython.schedule(schedule_watchdog, end_ticks)


async def test(ai):
    for x in range(3):
        await asyncio.sleep(0.1)
        ai.fetch_data(f"bar {x}")


class AsyncIterable:
    def __init__(self):
        self.message = None
        self.evt = asyncio.Event()

    def __aiter__(self):
        return self

    async def __anext__(self):
        await self.evt.wait()
        self.evt.clear()
        if self.message is None:
            raise StopAsyncIteration
        return self.message

    def fetch_data(self, message):
        self.message = message
        self.evt.set()


async def main():
    global ai
    ai = AsyncIterable()
    asyncio.create_task(test(ai))
    schedule_watchdog(time.ticks_add(time.ticks_ms(), 500))
    async for message in ai:
        print(message)
    print("end main")


asyncio.run(main())
