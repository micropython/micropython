try:
    import uasyncio as asyncio
except:
    import asyncio


async def wait():
    print("waiting")
    try:
        await asyncio.sleep(0.3)
    except asyncio.CancelledError:
        print("wait cancelled")
        raise
    print("waiting exited")
    return True


async def awaiter(t):
    print("awaiting t")
    try:
        r = await asyncio.shield(t)
    except asyncio.CancelledError:
        print("awaiting cancelled")
        raise
    print(r)
    return True


async def test_await_shield():
    print(await awaiter(wait()))
    t = asyncio.create_task(awaiter(wait()))
    print(await t)


async def test_reuse_shield_task():
    async def test():
        print(await awaiter(wait()))
        await wait()

    t = asyncio.create_task(test())
    await asyncio.sleep(0.5)
    t.cancel()
    await asyncio.sleep(0.5)


async def test_cancel_wait():
    aw = asyncio.create_task(wait())
    t = []
    for _ in range(5):
        t.append(asyncio.create_task(awaiter(aw)))
    await asyncio.sleep(0.1)
    aw.cancel()
    await asyncio.sleep(0.1)
    print("stopping")
    await asyncio.sleep(0.1)


async def test_cancel_single_awaiter():
    aw = asyncio.create_task(wait())
    t = []
    for _ in range(5):
        t.append(asyncio.create_task(awaiter(aw)))
    await asyncio.sleep(0.1)
    t[0].cancel()
    await asyncio.sleep(0.5)
    print("stopping")


async def test_cancel_event():
    ev = asyncio.Event()

    async def test():
        try:
            await ev.wait()
        except asyncio.CancelledError:
            print("waiting Event cancelled")

    t = asyncio.create_task(test())
    await asyncio.sleep(0.1)
    t.cancel()
    await asyncio.sleep(0.1)
    print("stopping")


async def test_wait_for_shield():
    # tests shield when awaiting coroutine and not task
    try:
        print(await asyncio.wait_for(awaiter(wait()), 0.6))
    except asyncio.TimeoutError:
        print("timeout")
    await asyncio.sleep(0.5)
    try:
        print(await asyncio.wait_for(awaiter(wait()), 0.1))
    except asyncio.TimeoutError:
        print("timeout")
    await asyncio.sleep(0.5)


async def test_cancel_wait_for_shield():
    async def test():
        try:
            print(await asyncio.wait_for(awaiter(wait()), 0.6))
        except asyncio.TimeoutError:
            print("timeout")
        except asyncio.CancelledError:
            print("cancelled")

    t = asyncio.create_task(test())
    await asyncio.sleep(0.1)
    t.cancel()
    await asyncio.sleep(0.1)


print("----------------------------\ntest_await_shield")
asyncio.run(test_await_shield())
print("----------------------------\ntest_cancel_event")
asyncio.run(test_cancel_event())
print("----------------------------\ntest_cancel_wait")
asyncio.run(test_cancel_wait())
print("----------------------------\ntest_cancel_single_awaiter")
asyncio.run(test_cancel_single_awaiter())
print("----------------------------\ntest_wait_for_shield")
asyncio.run(test_wait_for_shield())
print("----------------------------\ntest_cancel_wait_for_shield")
asyncio.run(test_cancel_wait_for_shield())
print("----------------------------\ntest_reuse_shield_task")
asyncio.run(test_reuse_shield_task())
