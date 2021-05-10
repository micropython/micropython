# test raising and catching an exception within a thread
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import _thread


def foo():
    raise ValueError


def thread_entry():
    try:
        foo()
    except ValueError:
        pass
    with lock:
        global n_finished
        n_finished += 1


lock = _thread.allocate_lock()
n_thread = 4
n_finished = 0

# spawn threads
for i in range(n_thread):
    while True:
        try:
            _thread.start_new_thread(thread_entry, ())
            break
        except OSError:
            pass

# busy wait for threads to finish
while n_finished < n_thread:
    pass
print("done")
