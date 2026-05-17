# test basic capability to start a new thread
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import time
import _thread


def foo():
    pass


def thread_entry(n):
    for i in range(n):
        foo()


for i in range(2):
    while True:
        try:
            _thread.start_new_thread(thread_entry, ((i + 1) * 10,))
            break
        except OSError:
            pass

# wait for threads to finish
time.sleep(1)
print("done")
