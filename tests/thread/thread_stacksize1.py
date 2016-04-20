# test setting the thread stack size
#
# MIT license; Copyright (c) 2016 Damien P. George on behalf of Pycom Ltd

import sys
try:
    import utime as time
except ImportError:
    import time
import _thread

# different implementations have different minimum sizes
if sys.implementation == 'micropython':
    sz = 2 * 1024
else:
    sz = 32 * 1024

def foo():
    pass

def thread_entry():
    foo()

# test set/get of stack size
print(_thread.stack_size())
print(_thread.stack_size(sz))
print(_thread.stack_size() == sz)
print(_thread.stack_size())

# set stack size and spawn a few threads
_thread.stack_size(sz)
for i in range(2):
    _thread.start_new_thread(thread_entry, ())

time.sleep(0.2)
print('done')
