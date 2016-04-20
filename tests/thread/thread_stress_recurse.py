# test hitting the function recursion limit within a thread
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

try:
    import utime as time
except ImportError:
    import time
import _thread

def foo():
    foo()

def thread_entry():
    try:
        foo()
    except RuntimeError:
        print('RuntimeError')

_thread.start_new_thread(thread_entry, ())

time.sleep(0.2)
print('done')
