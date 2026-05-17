# Ensure that an asyncio task can wait on an Event when the
# _task_queue is empty
# https://github.com/micropython/micropython/issues/16569

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


evt = asyncio.Event()


def schedule_watchdog(end_ticks):
    if time.ticks_diff(end_ticks, time.ticks_ms()) <= 0:
        print("asyncio still pending, unlocking event")
        # Caution: about to call Event.set() from a schedule
        #          (see the note in the comment above)
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
