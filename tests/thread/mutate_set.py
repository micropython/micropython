# test concurrent mutating access to a shared set object
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import _thread

# the shared set
se = set([-1, -2, -3, -4])


# main thread function
def th(n, lo, hi):
    for repeat in range(n):
        for i in range(lo, hi):
            se.add(i)
            assert i in se

            se.remove(i)
            assert i not in se

    with lock:
        global n_finished
        n_finished += 1


lock = _thread.allocate_lock()
n_thread = 4
n_finished = 0

# spawn threads
for i in range(n_thread):
    _thread.start_new_thread(th, (50, i * 500, (i + 1) * 500))

# busy wait for threads to finish
while n_finished < n_thread:
    pass

# check set has correct contents
print(sorted(se))
