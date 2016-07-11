# test concurrent mutating access to a shared dict object
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import _thread

# the shared dict
di = {'a':'A', 'b':'B', 'c':'C', 'd':'D'}

# main thread function
def th(n, lo, hi):
    for repeat in range(n):
        for i in range(lo, hi):
            di[i] = repeat + i
            assert di[i] == repeat + i

            del di[i]
            assert i not in di

            di[i] = repeat + i
            assert di[i] == repeat + i

            assert di.pop(i) == repeat + i

    with lock:
        global n_finished
        n_finished += 1

lock = _thread.allocate_lock()
n_thread = 4
n_finished = 0

# spawn threads
for i in range(n_thread):
    _thread.start_new_thread(th, (30, i * 300, (i + 1) * 300))

# busy wait for threads to finish
while n_finished < n_thread:
    pass

# check dict has correct contents
print(sorted(di.items()))
