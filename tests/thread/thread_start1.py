# test basic capability to start a new thread
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

try:
    import utime as time
except ImportError:
    import time
import _thread


def foo():
    pass


def thread_entry(n):
    for i in range(n):
        foo()


_thread.start_new_thread(thread_entry, (10,))
_thread.start_new_thread(thread_entry, (20,))

# wait for threads to finish
time.sleep(1)
print("done")
