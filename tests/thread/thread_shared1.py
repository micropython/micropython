# test capability for threads to access a shared immutable data structure
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

try:
    import utime as time
except ImportError:
    import time
import _thread

def foo(i):
    pass

def thread_entry(n, tup):
    for i in tup:
        foo(i)

tup = (1, 2, 3, 4)
_thread.start_new_thread(thread_entry, (100, tup))
_thread.start_new_thread(thread_entry, (100, tup))

# wait for threads to finish
time.sleep(0.2)
print(tup)
