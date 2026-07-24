# Test time.sleep() handling of scheduled callbacks and negative values.

try:
    import micropython

    micropython.schedule
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

import time

# A negative sleep raises ValueError. Which numeric branch this exercises
# (float or integer) depends on the build's float support.
try:
    time.sleep(-1)
    print("FAIL: no ValueError")
except ValueError:
    print("ValueError")

# A callback scheduled before the sleep is processed during the sleep call.
result = []
micropython.schedule(lambda _: result.append(1), None)
time.sleep(0)
print("callback:", result)

# Sleep still sleeps for the requested duration.
# Compute the float from integers at runtime as some builds lack float literals.
start = time.ticks_ms()
time.sleep(50 / 1000)
elapsed = time.ticks_diff(time.ticks_ms(), start)
if 40 <= elapsed < 500:
    print("timing ok")
else:
    print("timing FAIL:", elapsed, "ms")
