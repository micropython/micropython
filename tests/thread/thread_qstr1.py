# test concurrent interning of strings
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

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
n_thread = 4
n_finished = 0

# spawn threads
for i in range(n_thread):
    _thread.start_new_thread(th, (i * 1000, 1000))

# busy wait for threads to finish
while n_finished < n_thread:
    pass

print('pass')
