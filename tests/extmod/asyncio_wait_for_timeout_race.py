# Test asyncio.wait_for() when aw finishes at the same time as the timeout
# expires.  The outcome of aw (result or exception) must not be lost, and a
# genuine timeout must still raise TimeoutError.
# https://github.com/micropython/micropython/issues/19779
# requires: async

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit

import time

TIMEOUT = 0.05


async def run(outcome):
    ev = asyncio.Event()

    async def aw():
        await ev.wait()
        if outcome == "raise":
            raise ValueError("boom")
        if outcome == "cancel":
            # Finish with CancelledError, as if aw were cancelled.
            raise asyncio.CancelledError
        return "result"

    async def trigger():
        await asyncio.sleep(0.01)
        ev.set()  # schedules aw, which will finish before wait_for wakes up
        time.sleep(2 * TIMEOUT)  # block the scheduler past the timeout

    if outcome != "timeout":
        # In the "timeout" case ev is never set, so aw never finishes.
        asyncio.create_task(trigger())

    try:
        print(outcome, "->", await asyncio.wait_for(aw(), TIMEOUT))
    except BaseException as er:
        print(outcome, "->", type(er).__name__)


async def main():
    # aw returns a value at the same time as the timeout: value must be returned.
    await run("return")
    # aw raises at the same time as the timeout: its exception must propagate.
    await run("raise")
    # aw is cancelled at the same time as the timeout: CancelledError must propagate.
    await run("cancel")
    # aw really doesn't finish in time: TimeoutError must still be raised.
    await run("timeout")


asyncio.run(main())
