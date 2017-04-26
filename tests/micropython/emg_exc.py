# test that emergency exceptions work

import micropython
import sys

# some ports need to allocate heap for the emg exc
try:
    micropython.alloc_emergency_exception_buf(256)
except AttributeError:
    pass

def f():
    micropython.heap_lock()
    try:
        raise ValueError(1)
    except ValueError as er:
        sys.print_exception(er)
    micropython.heap_unlock()

f()
