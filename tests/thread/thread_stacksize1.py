# test setting the thread stack size
#
# SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd
#
# SPDX-License-Identifier: MIT

import sys
import _thread

# different implementations have different minimum sizes
if sys.implementation.name == "micropython":
    sz = 2 * 1024
else:
    sz = 512 * 1024


def foo():
    pass


def thread_entry():
    foo()
    with lock:
        global n_finished
        n_finished += 1


# reset stack size to default
_thread.stack_size()

# test set/get of stack size
print(_thread.stack_size())
print(_thread.stack_size(sz))
print(_thread.stack_size() == sz)
print(_thread.stack_size())

lock = _thread.allocate_lock()
n_thread = 2
n_finished = 0

# set stack size and spawn a few threads
_thread.stack_size(sz)
for i in range(n_thread):
    while True:
        try:
            _thread.start_new_thread(thread_entry, ())
            break
        except OSError:
            pass

# reset stack size to default (for subsequent scripts on baremetal)
_thread.stack_size()

# busy wait for threads to finish
while n_finished < n_thread:
    pass
print("done")
