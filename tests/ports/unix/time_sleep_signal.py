# Test time.sleep() signal-based scheduler integration on unix port.
#
# The signal-based EINTR wakeup during select() cannot be reliably tested
# from Python because kill(getpid()) delivers to an arbitrary thread, and
# the scheduling thread may receive it instead of the sleeping thread.
# Instead, test the drain loop (processes already-pending callbacks before
# sleeping) and ValueError for negative values.

import micropython
import time

# Test 1: ValueError for negative sleep.
try:
    time.sleep(-1)
    print("FAIL: no ValueError")
except ValueError:
    print("ValueError")

# Test 2: Pending callbacks are processed during time.sleep().
# Schedule a chain of callbacks from within a callback so they accumulate
# while the scheduler is locked. time.sleep()'s drain loop should process
# them all before entering select().
results = []


def callback(n):
    results.append(n)
    if n < 3:
        micropython.schedule(callback, n + 1)


micropython.schedule(callback, 0)

# Small sleep to allow drain loop to process the chain.
time.sleep(0.01)

# All callbacks in the chain should have been processed.
print("callbacks:", sorted(results))

# Test 3: Basic sleep timing still works (not broken by signal changes).
start = time.ticks_ms()
time.sleep(0.05)
elapsed = time.ticks_diff(time.ticks_ms(), start)
# Should be at least 40ms (allowing for timing imprecision) and under 500ms.
if 40 <= elapsed < 500:
    print("timing ok")
else:
    print("timing FAIL:", elapsed, "ms")
