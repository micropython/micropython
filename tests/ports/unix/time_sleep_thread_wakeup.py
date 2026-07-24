# Test that a callback scheduled from another thread wakes a blocking
# time.sleep() promptly, and that the sleep still runs to completion.

try:
    import _thread
    import micropython

    micropython.schedule
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit

import time

cb_at = []
start = time.ticks_ms()


def cb(_):
    cb_at.append(time.ticks_diff(time.ticks_ms(), start))


def worker():
    time.sleep_ms(50)
    micropython.schedule(cb, None)


_thread.start_new_thread(worker, ())
time.sleep(500 / 1000)
total = time.ticks_diff(time.ticks_ms(), start)

# The callback should run near the 50ms schedule point, well before the
# sleep finishes.
if cb_at and cb_at[0] < 300:
    print("callback prompt")
else:
    print("callback FAIL:", cb_at)

# The sleep must still complete its full duration.
if 500 <= total < 2000:
    print("duration ok")
else:
    print("duration FAIL:", total)
