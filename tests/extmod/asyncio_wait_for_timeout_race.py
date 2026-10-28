# Test asyncio.wait_for() when aw finishes at the same time as the timeout
# expires.  The outcome of aw (result or exception) must not be lost, and a
# genuine timeout must still raise TimeoutError.
#
# The race is made deterministic by blocking the scheduler: aw is woken well
# before the timeout, but the scheduler is then blocked until after the timeout
# has expired.  aw then runs to completion first, leaving wait_for's runner
# task still pending when wait_for's own sleep wakes up.

try:
    import asyncio
    import time

    time.sleep_ms
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

TIMEOUT_MS = 50


async def run(outcome):
    ev = asyncio.Event()

    async def aw():
        await ev.wait()
        if outcome == "raise":
            raise ValueError("boom")
        return "result"

    async def trigger():
        await asyncio.sleep_ms(10)
        ev.set()  # schedules aw, which will finish before wait_for wakes up
        time.sleep_ms(2 * TIMEOUT_MS)  # block the scheduler past the timeout

    if outcome != "timeout":
        # In the "timeout" case ev is never set, so aw never finishes.
        asyncio.create_task(trigger())

    try:
        print(outcome, "->", await asyncio.wait_for_ms(aw(), TIMEOUT_MS))
    except Exception as er:
        print(outcome, "->", type(er).__name__)


async def main():
    # aw returns a value at the same time as the timeout: value must be returned.
    await run("return")
    # aw raises at the same time as the timeout: its exception must propagate.
    await run("raise")
    # aw really doesn't finish in time: TimeoutError must still be raised.
    await run("timeout")


asyncio.run(main())
