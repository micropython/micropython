# test concurrent mutating access to a shared list object
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import time
import _thread

# the shared list
li = list()


# main thread function
def th(n, lo, hi):
    for repeat in range(n):
        for i in range(lo, hi):
            li.append(i)
            assert li.count(i) == repeat + 1

            li.extend([i, i])
            assert li.count(i) == repeat + 3

            li.remove(i)
            assert li.count(i) == repeat + 2

            li.remove(i)
            assert li.count(i) == repeat + 1

    with lock:
        global n_finished
        n_finished += 1


lock = _thread.allocate_lock()
n_thread = 4
n_finished = 0

# spawn threads
for i in range(n_thread):
    _thread.start_new_thread(th, (4, i * 60, (i + 1) * 60))

# busy wait for threads to finish
while n_finished < n_thread:
    time.sleep(0)

# check list has correct contents
li.sort()
print(li)
