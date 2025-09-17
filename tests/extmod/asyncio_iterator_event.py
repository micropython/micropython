# Ensure that an asyncio task can wait on an Event when the
# _task_queue is empty, in the context of an async iterator
# https://github.com/micropython/micropython/issues/16318

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit

# This test requires checking that the asyncio scheduler
# remains active "indefinitely" when the task queue is empty.
#
# To check this, we need another independent scheduler that
# can wait for a certain amount of time. So we have to
# create one using micropython.schedule() and time.ticks_ms()
#
# Technically, this code breaks the rules, as it is clearly
# documented that Event.set() should _NOT_ be called from a
# schedule (soft IRQ) because in some cases, a race condition
# can occur, resulting in a crash. However:
# - since the risk of a race condition in that specific
#   case has been analysed and excluded
# - given that there is no other simple alternative to
#   write this test case,
# an exception to the rule was deemed acceptable. See
# https://github.com/micropython/micropython/pull/16772

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
        # Caution: ai.fetch_data() will invoke Event.set()
        #          (see the note in the comment above)
        ai.fetch_data(None)
        return
    micropython.schedule(schedule_watchdog, end_ticks)


async def test(ai):
    for x in range(3):
        await asyncio.sleep(0.1)
        ai.fetch_data("bar {}".format(x))


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
