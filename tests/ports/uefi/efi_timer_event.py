# uefi.event: a one-shot EVT_TIMER waited on via the WFE, and Event.wait_async — an
# asyncio task that suspends until another coroutine signals the event.
try:
    import uefi
    import asyncio
    from uefi import event as EV
except ImportError:
    print("SKIP")
    raise SystemExit

t = EV.Timer()
t.set_oneshot(30)
print(t.wait(2000))  # fires well within 2 s
t.close()

ev = uefi.Event()


async def fire():
    await asyncio.sleep_ms(20)
    ev.signal()


async def amain():
    await asyncio.gather(fire(), EV.wait_async(ev))


asyncio.run(amain())
print(ev.is_signaled())
ev.close()
print("timer/event async ok")
