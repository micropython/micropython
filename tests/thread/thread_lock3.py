# test thread coordination using a lock object
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import _thread

lock = _thread.allocate_lock()
n_thread = 10
n_finished = 0


def thread_entry(idx):
    global n_finished
    while True:
        with lock:
            if n_finished == idx:
                break
    print("my turn:", idx)
    with lock:
        n_finished += 1


# spawn threads
for i in range(n_thread):
    _thread.start_new_thread(thread_entry, (i,))

# busy wait for threads to finish
while n_finished < n_thread:
    pass
