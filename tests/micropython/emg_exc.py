# test that emergency exceptions work

import micropython
import sys

try:
    import uio
except ImportError:
    print("SKIP")
    raise SystemExit

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
        exc = er
    micropython.heap_unlock()

    print(repr(exc))


f()
