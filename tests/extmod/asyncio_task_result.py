# Test the Task.result() method

try:
    import asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def task(t, exc=None, ret=None):
    if t >= 0:
        await asyncio.sleep(t)
    if exc:
        raise exc
    return ret


async def main():
    # Task that is not done yet raises an InvalidStateError
    print("=" * 10)
    t = asyncio.create_task(task(1))
    await asyncio.sleep(0)
    try:
        t.result()
        assert False, "Should not get here"
    except Exception as e:
        print("InvalidStateError if still running:", repr(e))

    # Task that is cancelled raises CancelledError
    print("=" * 10)
    t = asyncio.create_task(task(1))
    t.cancel()
    await asyncio.sleep(0)
    try:
        t.result()
        assert False, "Should not get here"
    except asyncio.CancelledError as e:
        print("CancelledError when retrieving result from cancelled task:", repr(e))

    # Task that raises immediately should raise that exception when calling result
    print("=" * 10)
    t = asyncio.create_task(task(-1, ValueError))
    try:
        await t
        assert False, "Should not get here"
    except ValueError as e:
        pass

    try:
        t.result()
        assert False, "Should not get here"
    except ValueError as e:
        print("Error raised when result is attempted on task with error:", repr(e))

    # Task that starts, runs and finishes without an exception or value should return None
    print("=" * 10)
    t = asyncio.create_task(task(0.01))
    await t
    print("Empty Result should be None:", t.result())
    assert t.result() is None

    # Task that starts, runs and finishes without exception should return result
    print("=" * 10)
    t = asyncio.create_task(task(0.01, None, "hello world"))
    await t
    print("Happy path, result is returned:", t.result())


asyncio.run(main())
