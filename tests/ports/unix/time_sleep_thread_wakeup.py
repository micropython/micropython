# Test that a callback scheduled from another thread wakes a blocking
# time.sleep() promptly, and that the sleep still runs to completion.

try:
    import _thread
    import micropython

    micropython.schedule
    # The sleeps below are given float lengths.
    50 / 1000
except (ImportError, AttributeError, TypeError):
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
if cb_at and cb_at[0] < 400:
    print("callback prompt")
else:
    print("callback FAIL:", cb_at)

# The sleep must still complete its full duration.
if 500 <= total < 2000:
    print("duration ok")
else:
    print("duration FAIL:", total)

# A sleep on a non-main thread must also honour its full duration. Without a
# GIL such a thread cannot run scheduled callbacks so it does not wait on the
# wake event at all; with one it does, and re-enters the wait until its
# deadline.
slept = []


def sleeper():
    t0 = time.ticks_ms()
    time.sleep(200 / 1000)
    slept.append(time.ticks_diff(time.ticks_ms(), t0))


_thread.start_new_thread(sleeper, ())
# Queue work while the other thread is inside its sleep.
time.sleep_ms(50)
micropython.schedule(lambda _: None, None)
time.sleep_ms(400)

if slept and 200 <= slept[0] < 1000:
    print("thread sleep ok")
else:
    print("thread sleep FAIL:", slept)
