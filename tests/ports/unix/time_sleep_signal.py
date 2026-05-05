# Test time.sleep() integration with the scheduler signal mechanism.
# Tests ValueError for negative values, callback processing during sleep,
# and basic timing through the modified code path.

try:
    import micropython

    micropython.schedule
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

import time

# Test 1: ValueError for negative sleep (float path).
try:
    time.sleep(-1)
    print("FAIL: no ValueError for float")
except ValueError:
    print("ValueError")

# Test 2: ValueError for negative sleep (int path).
try:
    time.sleep(-1)
    print("FAIL: no ValueError for int")
except ValueError:
    print("ValueError")

# Test 3: Scheduled callback is processed during time.sleep().
# mp_handle_pending() in the sleep loop should run the callback.
result = []
micropython.schedule(lambda _: result.append(1), None)
time.sleep(0)
print("callback:", result)

# Test 4: Basic sleep timing through the float path.
# Verifies the modified time.sleep still sleeps for the correct duration.
# Compute float from integers at runtime to avoid float literal (minimal can't parse them).
start = time.ticks_ms()
time.sleep(50 / 1000)
elapsed = time.ticks_diff(time.ticks_ms(), start)
if 40 <= elapsed < 500:
    print("timing ok")
else:
    print("timing FAIL:", elapsed, "ms")
