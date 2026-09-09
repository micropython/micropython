from machine import Timer
import machine
import time

fired = False


def _cb(_):
    global fired
    fired = True


irq_state = machine.disable_irq()
tim = Timer(0, period=20, mode=Timer.ONE_SHOT, callback=_cb)

# Busy-wait with IRQs disabled (time.ticks_* is paused on this port).
for _ in range(500000):
    pass

# Callback should still be blocked.
print("while_disabled:", fired)

machine.enable_irq(irq_state)
time.sleep_ms(200)

# Callback should run after IRQ restore.
print("after_enable:", fired)

tim.deinit()
