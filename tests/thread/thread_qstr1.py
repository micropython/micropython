# test concurrent interning of strings
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import time
import _thread


# function to check the interned string
def check(s, val):
    assert type(s) == str
    assert int(s) == val


# main thread function
def th(base, n):
    for i in range(n):
        # this will intern the string and check it
        exec("check('%u', %u)" % (base + i, base + i))

    with lock:
        global n_finished
        n_finished += 1


lock = _thread.allocate_lock()
n_thread = 0
n_thread_max = 4
n_finished = 0
n_qstr_per_thread = 100  # make 1000 for a more stressful test (uses more heap)

# spawn threads
for _ in range(n_thread_max):
    try:
        _thread.start_new_thread(th, (n_thread * n_qstr_per_thread, n_qstr_per_thread))
        n_thread += 1
    except OSError:
        # System cannot create a new thead, so stop trying to create them.
        break

# also run the function on this main thread
th(n_thread * n_qstr_per_thread, n_qstr_per_thread)
n_thread += 1

# wait for threads to finish
while n_finished < n_thread:
    time.sleep(0)

print("pass")
