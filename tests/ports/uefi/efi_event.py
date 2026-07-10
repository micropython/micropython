# efi.Event: signal / clear / wait, select-pollability, and cross-signalling from a
# machine.Timer callback (an external signal funnelling through the notify/wakeup loop).
try:
    import efi
    import select
    import machine
except ImportError:
    print("SKIP")
    raise SystemExit

ev = efi.Event()
print(ev.is_signaled())
ev.signal()
print(ev.is_signaled())
print(ev.wait(0))  # already signalled
ev.clear()
print(ev.is_signaled())
print(ev.wait(40))  # times out (rides the WFE)

# select-pollable
pe = select.poll()
pe.register(ev, select.POLLIN)
print(pe.poll(0))  # not signalled -> not ready
ev.signal()
print(len(pe.poll(0)))  # signalled -> ready

# A one-shot timer callback signals a second event; a blocking wait() returns when it fires.
ev2 = efi.Event()
t = machine.Timer(-1, period=30, mode=machine.Timer.ONE_SHOT, callback=lambda t: ev2.signal())
print(ev2.wait(2000))
ev.close()
ev2.close()
t.deinit()
print("efi.Event ok")
