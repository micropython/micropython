# test raising and catching an exception within a thread
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

try:
    import utime as time
except ImportError:
    import time
import _thread

def foo():
    raise ValueError

def thread_entry():
    try:
        foo()
    except ValueError:
        pass

for i in range(4):
    _thread.start_new_thread(thread_entry, ())

time.sleep(0.2)
print('done')
