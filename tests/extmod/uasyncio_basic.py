try:
    import uasyncio as asyncio
except ImportError:
    try:
        import asyncio
    except ImportError:
        print("SKIP")
        raise SystemExit


try:
    import utime

    ticks = utime.ticks_ms
    ticks_diff = utime.ticks_diff
except:
    import time

    ticks = lambda: int(time.time() * 1000)
    ticks_diff = lambda t1, t0: t1 - t0


async def delay_print(t, s):
    await asyncio.sleep(t)
    print(s)


async def main():
    print("start")

    await asyncio.sleep(0.001)
    print("after sleep")

    t0 = ticks()
    await delay_print(0.2, "short")
    t1 = ticks()
    await delay_print(0.4, "long")
    t2 = ticks()
    await delay_print(-1, "negative")
    t3 = ticks()

    print(
        "took {} {} {}".format(
            round(ticks_diff(t1, t0), -2),
            round(ticks_diff(t2, t1), -2),
            round(ticks_diff(t3, t2), -2),
        )
    )


asyncio.run(main())
