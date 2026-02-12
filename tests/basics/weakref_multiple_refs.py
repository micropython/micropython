# Test weakref when multiple weak references are active.
#
# This test has different output to CPython due to the order that MicroPython
# executes weak reference callbacks.

try:
    import weakref
except ImportError:
    print("SKIP")
    raise SystemExit

# gc module must be available if weakref is.
import gc


class A:
    def __str__(self):
        return "<A object>"


def test():
    print("test having multiple ref and finalize objects referencing the same thing")
    a = A()
    r1 = weakref.ref(a, lambda r: print("ref1", r()))
    f1 = weakref.finalize(a, lambda: print("finalize1"))
    r2 = weakref.ref(a, lambda r: print("ref2", r()))
    f2 = weakref.finalize(a, lambda: print("finalize2"))
    print(r1(), f1.alive, r2(), f2.alive)
    a = None
    clean_the_stack = [0, 0, 0, 0]
    gc.collect()


test()
