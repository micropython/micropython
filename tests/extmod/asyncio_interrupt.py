try:
    import time
    import asyncio
    import machine

    machine.Timer
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


async def block(n):  # Blocking task
    while True:
        time.sleep_ms(5)
        await asyncio.sleep_ms(0)


async def timer_test(iters=5, target_ms=100, err_limit_us=100):
    target_us = target_ms * 1000

    tasks = []
    for n in range(10):
        tasks.append(asyncio.create_task(block(n)))

    irq = asyncio.Interrupt()
    tim = machine.Timer()

    for x in range(iters):
        t0_us = time.ticks_us()
        tim.init(mode=machine.Timer.ONE_SHOT, period=target_ms, callback=irq, hard=False)
        async with irq:  # body of this with-block runs as an interrupt handler, with arg set to the value passed to irq()
            t1_us = time.ticks_us()
        tim.deinit()

        dt_us = time.ticks_diff(t1_us, t0_us)
        err_us = dt_us - target_us

        if 0 <= err_us < err_limit_us:
            print("{} OK <{}".format(x, err_limit_us))
        else:
            print("{} FAIL {}".format(x, err_us))

    tim.init(mode=machine.Timer.ONE_SHOT, period=target_ms, callback=irq, hard=False)
    try:
        async with irq as arg:
            assert arg is tim
            raise Exception("test")
    except Exception as e:
        assert e.args[0] == "test"
        print("Exception OK")
    tim.deinit()

    tim.init(mode=machine.Timer.ONE_SHOT, period=target_ms, callback=irq, hard=False)
    async with irq as arg:
        await asyncio.sleep(0)
        print("Premature Exit OK")
    tim.deinit()


asyncio.run(timer_test())
