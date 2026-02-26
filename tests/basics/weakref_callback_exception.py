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


def throw(*args):
    raise ValueError("weakref callback", args)
def noop(*args):
    pass

test_table = [
    (weakref.ref, noop, None),
    (weakref.ref, throw, "test ref with exception in the callback"),
    (weakref.ref, noop, None),
    (weakref.ref, noop, "collect done"),
    (weakref.finalize, noop, None),
    (weakref.finalize, throw, "test finalize with exception in the callback"),
    (weakref.finalize, noop, None),
    (weakref.finalize, noop, "collect done"),
]


def test():
    for RefType, callback, msg in test_table:
        if msg:
            print(msg)
        a = A()
        r = RefType(a, callback)
        a = None

        clean_the_stack = [0, 0, 0, 0]
        gc.collect()
        try:
            raise Exception
        except Exception:
            pass
        gc.collect()

test()
