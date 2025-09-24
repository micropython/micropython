# test that we can generate a traceback without allocating

import sys

try:
    import io, micropython
except ImportError:
    print("SKIP")
    raise SystemExit

# preallocate exception instance with some room for a traceback
global_exc = StopIteration()
try:
    raise global_exc
except:
    pass


def test():
    micropython.heap_lock()
    global global_exc
    global_exc.__traceback__ = None
    try:
        raise global_exc
    except StopIteration:
        print("StopIteration")
    micropython.heap_unlock()


# call test() with heap allocation disabled
test()

# print the exception that was raised
buf = io.StringIO()
sys.print_exception(global_exc, buf)
for l in buf.getvalue().split("\n"):
    # MicroPython on pyboard prints <stdin> as file, so remove filename.
    if l.startswith("  File "):
        l = l.split('"')
        print(l[0], l[2])
    else:
        print(l)
