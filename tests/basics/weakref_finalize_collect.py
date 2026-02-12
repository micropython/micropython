# Test weakref.finalize() functionality requiring gc.collect().

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


def callback(*args, **kwargs):
    print("callback({}, {})".format(args, kwargs))
    return 42


def test():
    print("test basic use of finalize() with a simple callback")
    a = A()
    f = weakref.finalize(a, callback)
    a = None
    clean_the_stack = [0, 0, 0, 0]
    gc.collect()
    print("alive", f.alive)
    print("peek", f.peek())
    print("detach", f.detach())
    print("call", f())

    print("test that a callback is passed the correct values")
    a = A()
    f = weakref.finalize(a, callback, 1, 2, kwarg=3)
    a = None
    clean_the_stack = [0, 0, 0, 0]
    gc.collect()
    print("alive", f.alive)
    print("peek", f.peek())
    print("detach", f.detach())
    print("call", f())

    print("test that calling the finalizer cancels the finalizer")
    a = A()
    f = weakref.finalize(a, callback)
    print(f())
    print(a)
    a = None
    clean_the_stack = [0, 0, 0, 0]
    gc.collect()

    print("test that calling detach cancels the finalizer")
    a = A()
    f = weakref.finalize(a, callback)
    print(len(f.detach()))
    print(a)
    a = None
    clean_the_stack = [0, 0, 0, 0]
    gc.collect()

    print("test that finalize does not get collected before its ref does")
    a = A()
    weakref.finalize(a, callback)
    clean_the_stack = [0, 0, 0, 0]
    gc.collect()
    print("free a")
    a = None
    clean_the_stack = [0, 0, 0, 0]
    gc.collect()


test()
