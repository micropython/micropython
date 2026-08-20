try:
    import time
    import asyncio
    import machine

    machine.Timer
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class MillisecTimer:
    def __init__(self):
        self.tsf = asyncio.ThreadSafeFlag()
        self.tim = machine.Timer()

    def isr(self, tim):
        self.tsf.set()

    async def __call__(self, ms):
        self.tim.init(
            mode=machine.Timer.ONE_SHOT,
            period=ms,
            callback=self.isr,
            hard=True,
        )
        await self.wait(priority=True)


async def block(n):  # Blocking task
    while True:
        time.sleep_ms(5)
        await asyncio.sleep_ms(0)


async def timer_test(iters, target_ms, err_limit_us=10_000):
    target_us = target_ms * 1000

    tasks = []
    for n in range(10):
        tasks.append(asyncio.create_task(block(n)))

    for x in range(iters):
        timer = MillisecTimer()

        t0_us = time.ticks_us()
        await timer(target_ms)
        t1_us = time.ticks_us()

        dt_us = time.ticks_diff(t1_us, t0_us)
        err_us = dt_us - target_us

        if 0 <= err_us < err_limit_us:
            print("{} OK <{}".format(x, err_limit_us))
        else:
            print("{} FAIL {}".format(x, err_us))


asyncio.run(timer_test(5, 100))
