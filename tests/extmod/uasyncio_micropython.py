# Test MicroPython extensions on CPython asyncio:
# - sleep_ms
# - wait_for_ms

try:
    import time, uasyncio
except ImportError:
    print("SKIP")
    raise SystemExit


async def task(id, t):
    print("task start", id)
    await uasyncio.sleep_ms(t)
    print("task end", id)
    return id * 2


async def main():
    # Simple sleep_ms
    t0 = time.ticks_ms()
    await uasyncio.sleep_ms(1)
    print(time.ticks_diff(time.ticks_ms(), t0) < 100)

    try:
        # Sleep 1ms beyond maximum allowed sleep value
        await uasyncio.sleep_ms(time.ticks_add(0, -1) // 2 + 1)
    except OverflowError:
        print("OverflowError")

    # When task finished before the timeout
    print(await uasyncio.wait_for_ms(task(1, 5), 50))

    # When timeout passes and task is cancelled
    try:
        print(await uasyncio.wait_for_ms(task(2, 50), 5))
    except uasyncio.TimeoutError:
        print("timeout")

    print("finish")


uasyncio.run(main())
