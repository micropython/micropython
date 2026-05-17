# stress test for the heap by allocating lots of objects within threads
# allocates about 5mb on the heap
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import time
import _thread


def last(l):
    return l[-1]


def thread_entry(n):
    # allocate a bytearray and fill it
    data = bytearray(i for i in range(256))

    # run a loop which allocates a small list and uses it each iteration
    lst = 8 * [0]
    sum = 0
    for i in range(n):
        sum += last(lst)
        lst = [0, 0, 0, 0, 0, 0, 0, i + 1]

    # check that the bytearray still has the right data
    for i, b in enumerate(data):
        assert i == b

    # print the result of the loop and indicate we are finished
    with lock:
        print(sum, lst[-1])
        global n_finished
        n_finished += 1


lock = _thread.allocate_lock()
n_thread = 10
n_finished = 0

# spawn threads
for i in range(n_thread):
    _thread.start_new_thread(thread_entry, (10000,))

# wait for threads to finish
while n_finished < n_thread:
    time.sleep(1)
