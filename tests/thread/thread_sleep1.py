# test threads sleeping
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

try:
    import utime as time
except ImportError:
    import time
import _thread

lock = _thread.allocate_lock()
n_thread = 16
n_finished = 0

def thread_entry(t):
    global n_finished
    time.sleep(t)
    time.sleep(2 * t)
    with lock:
        n_finished += 1

for i in range(n_thread):
    _thread.start_new_thread(thread_entry, (i / 100,))

# wait for threads to finish
while n_finished < n_thread:
    time.sleep(0.1)
print('done', n_thread)
