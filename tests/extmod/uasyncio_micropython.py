# Test MicroPython extensions on CPython asyncio:
# - sleep_ms
# - wait_for_ms

try:
    import utime, uasyncio
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
    t0 = utime.ticks_ms()
    await uasyncio.sleep_ms(1)
    print(utime.ticks_diff(utime.ticks_ms(), t0) < 100)

    # When task finished before the timeout
    print(await uasyncio.wait_for_ms(task(1, 5), 50))

    # When timeout passes and task is cancelled
    try:
        print(await uasyncio.wait_for_ms(task(2, 50), 5))
    except uasyncio.TimeoutError:
        print("timeout")

    print("finish")


uasyncio.run(main())
