# Test asyncio.wait_for, with forwarding cancellation

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def awaiting(t, return_if_fail):
    try:
        print("awaiting started")
        await asyncio.sleep(t)
    except asyncio.CancelledError as er:
        # CPython wait_for raises CancelledError inside task but TimeoutError in wait_for
        print("awaiting canceled")
        if return_if_fail:
            return False  # return has no effect if Cancelled
        else:
            raise er
    except Exception as er:
        print("caught exception", er)
        raise er


async def test_cancellation_forwarded(catch, catch_inside):
    print("----------")

    async def wait():
        try:
            await asyncio.wait_for(awaiting(2, catch_inside), 1)
        except asyncio.TimeoutError as er:
            print("Got timeout error")
            raise er
        except asyncio.CancelledError as er:
            print("Got canceled")
            if not catch:
                raise er

    async def cancel(t):
        print("cancel started")
        await asyncio.sleep(0.01)
        print("cancel wait()")
        t.cancel()

    t = asyncio.create_task(wait())
    k = asyncio.create_task(cancel(t))
    try:
        await t
    except asyncio.CancelledError:
        print("waiting got cancelled")


asyncio.run(test_cancellation_forwarded(False, False))
asyncio.run(test_cancellation_forwarded(False, True))
asyncio.run(test_cancellation_forwarded(True, True))
asyncio.run(test_cancellation_forwarded(True, False))
