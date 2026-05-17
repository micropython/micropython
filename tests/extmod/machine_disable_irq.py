# Test executing Python code while IRQs disabled.
try:
    from machine import disable_irq, enable_irq
    from time import ticks_us
except ImportError:
    print("SKIP")
    raise SystemExit


# Structured to also nest disable_irq() calls
def f(n):
    st_irq = disable_irq()
    if n:
        f(n - 1)
    else:
        # busy-wait in a tight loop for 1ms, to simulate doing some work in a critical section
        # (can't wait for time_us() to increment as not all ports will "tick" with interrupts off.)
        for _ in range(100):
            ticks_us()
    enable_irq(st_irq)


for nest in range(3):
    print(nest)
    for _ in range(5):
        f(nest)
