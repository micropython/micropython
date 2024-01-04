# test capability for threads to access a shared immutable data structure
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import _thread


def foo(i):
    pass


def thread_entry(n, tup):
    for i in tup:
        foo(i)
    with lock:
        global n_finished
        n_finished += 1


lock = _thread.allocate_lock()
n_thread = 0
n_thread_max = 2
n_finished = 0

# the shared data structure
tup = (1, 2, 3, 4)

# spawn threads
for _ in range(n_thread_max):
    try:
        _thread.start_new_thread(thread_entry, (100, tup))
        n_thread += 1
    except OSError:
        # System cannot create a new thead, so stop trying to create them.
        break

# also run the function on this main thread
thread_entry(100, tup)
n_thread += 1

# busy wait for threads to finish
while n_finished < n_thread:
    pass
print(tup)
