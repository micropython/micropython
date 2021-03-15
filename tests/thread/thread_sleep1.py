# test threads sleeping
#
# SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd
#
# SPDX-License-Identifier: MIT

try:
    import utime

    sleep_ms = utime.sleep_ms
except ImportError:
    import time

    sleep_ms = lambda t: time.sleep(t / 1000)

import _thread

lock = _thread.allocate_lock()
n_thread = 4
n_finished = 0


def thread_entry(t):
    global n_finished
    sleep_ms(t)
    sleep_ms(2 * t)
    with lock:
        n_finished += 1


for i in range(n_thread):
    _thread.start_new_thread(thread_entry, (10 * i,))

# wait for threads to finish
while n_finished < n_thread:
    sleep_ms(100)
print("done", n_thread)
