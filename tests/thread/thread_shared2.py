# test capability for threads to access a shared mutable data structure
# (without contention because they access different parts of the structure)
#
# SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd
#
# SPDX-License-Identifier: MIT

import _thread


def foo(lst, i):
    lst[i] += 1


def thread_entry(n, lst, idx):
    for i in range(n):
        foo(lst, idx)
    with lock:
        global n_finished
        n_finished += 1


lock = _thread.allocate_lock()
n_thread = 2
n_finished = 0

# the shared data structure
lst = [0, 0]

# spawn threads
for i in range(n_thread):
    _thread.start_new_thread(thread_entry, ((i + 1) * 10, lst, i))

# busy wait for threads to finish
while n_finished < n_thread:
    pass
print(lst)
