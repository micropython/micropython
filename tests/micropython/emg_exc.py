# test that emergency exceptions work

import micropython
import usys

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

    # print the exception
    buf = uio.StringIO()
    usys.print_exception(exc, buf)
    for l in buf.getvalue().split("\n"):
        if l.startswith("  File "):
            print(l.split('"')[2])
        else:
            print(l)


f()
