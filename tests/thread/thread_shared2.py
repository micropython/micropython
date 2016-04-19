# test capability for threads to access a shared mutable data structure
# (without contention because they access different parts of the structure)
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

try:
    import utime as time
except ImportError:
    import time
import _thread

def foo(lst, i):
    lst[i] += 1

def thread_entry(n, lst, idx):
    for i in range(n):
        foo(lst, idx)

lst = [0, 0]
_thread.start_new_thread(thread_entry, (10, lst, 0))
_thread.start_new_thread(thread_entry, (20, lst, 1))

# wait for threads to finish
time.sleep(0.2)
print(lst)
