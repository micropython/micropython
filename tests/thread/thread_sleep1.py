# test threads sleeping
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import time

if hasattr(time, "sleep_ms"):
    sleep_ms = time.sleep_ms
else:
    sleep_ms = lambda t: time.sleep(t / 1000)

import _thread

lock = _thread.allocate_lock()
n_thread = 0
n_thread_max = 4
n_finished = 0


def thread_entry(t):
    global n_finished
    sleep_ms(t)
    sleep_ms(2 * t)
    with lock:
        n_finished += 1


# spawn threads
for _ in range(n_thread_max):
    try:
        _thread.start_new_thread(thread_entry, (10 * n_thread,))
        n_thread += 1
    except OSError:
        # System cannot create a new thead, so stop trying to create them.
        break

# also run the function on this main thread
thread_entry(10 * n_thread)
n_thread += 1

# wait for threads to finish
while n_finished < n_thread:
    sleep_ms(100)
print("done")
