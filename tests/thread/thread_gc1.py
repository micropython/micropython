# test that we can run the garbage collector within threads
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import gc
import _thread


def thread_entry(n):
    # allocate a bytearray and fill it
    data = bytearray(i for i in range(256))

    # do some work and call gc.collect() a few times
    for i in range(n):
        for i in range(len(data)):
            data[i] = data[i]
        gc.collect()

    # check whether the data remains intact and indicate we are finished
    with lock:
        global n_correct, n_finished
        n_correct += list(data) == list(range(256))
        n_finished += 1


lock = _thread.allocate_lock()
n_thread = 0
n_thread_max = 4
n_correct = 0
n_finished = 0

# spawn threads
for _ in range(n_thread_max):
    try:
        _thread.start_new_thread(thread_entry, (10,))
        n_thread += 1
    except OSError:
        # System cannot create a new thead, so stop trying to create them.
        break

# also run the function on this main thread
thread_entry(10)
n_thread += 1

# busy wait for threads to finish
while n_finished < n_thread:
    pass

print(n_correct == n_finished)
