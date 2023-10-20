# Test MicroPython extensions on CPython asyncio:
# - sleep_ms
# - wait_for_ms

try:
    import time, asyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def task(id, t):
    print("task start", id)
    await asyncio.sleep_ms(t)
    print("task end", id)
    return id * 2


async def main():
    # Simple sleep_ms
    t0 = time.ticks_ms()
    await asyncio.sleep_ms(1)
    print(time.ticks_diff(time.ticks_ms(), t0) < 100)

    # When task finished before the timeout
    print(await asyncio.wait_for_ms(task(1, 5), 50))

    # When timeout passes and task is cancelled
    try:
        print(await asyncio.wait_for_ms(task(2, 50), 5))
    except asyncio.TimeoutError:
        print("timeout")

    print("finish")


asyncio.run(main())
