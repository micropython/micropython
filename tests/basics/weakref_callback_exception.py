# Test weakref ref/finalize raising an exception within the callback.
#
# This test has different output to CPython due to the way that MicroPython
# prints the exception.

try:
    import weakref
except ImportError:
    print("SKIP")
    raise SystemExit

import gc


class A:
    def __str__(self):
        return "<A object>"


def test():
    print("test ref with exception in the callback")
    a = A()
    r = weakref.ref(a, lambda r: 1 / 0)
    a = None
    clean_the_stack = [0, 0, 0, 0]
    gc.collect()
    print("collect done")

    print("test finalize with exception in the callback")
    a = A()
    weakref.finalize(a, lambda: 1 / 0)
    a = None
    clean_the_stack = [0, 0, 0, 0]
    gc.collect()
    print("collect done")


test()
