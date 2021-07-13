# test that we can run the garbage collector within threads
#
# SPDX-FileCopyrightText: Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd
#
# SPDX-License-Identifier: MIT

import gc
import _thread


def thread_entry(n):
    # allocate a bytearray and fill it
    data = bytearray(i for i in range(256))

    # do some work and call gc.collect() a few times
    for i in range(n):
        for i in range(len(data)):
            data[i] = data[i]
        gc.collect()

    # print whether the data remains intact and indicate we are finished
    with lock:
        print(list(data) == list(range(256)))
        global n_finished
        n_finished += 1


lock = _thread.allocate_lock()
n_thread = 4
n_finished = 0

# spawn threads
for i in range(n_thread):
    _thread.start_new_thread(thread_entry, (10,))

# busy wait for threads to finish
while n_finished < n_thread:
    pass
