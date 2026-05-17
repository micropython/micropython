# Test weakref.ref() functionality requiring gc.collect().
# Should be kept in sync with tests/ports/webassembly/weakref_ref_collect.py.

try:
    import weakref
except ImportError:
    print("SKIP")
    raise SystemExit

# gc module must be available if weakref is.
import gc

# Cannot reference non-heap objects.
for value in (None, False, True, Ellipsis, 0, "", ()):
    try:
        weakref.ref(value)
    except TypeError:
        print(value, "TypeError")


class A:
    def __str__(self):
        return "<A object>"


def callback(r):
    print("callback", r())


def test():
    print("test basic use of ref() with only one argument")
    a = A()
    r = weakref.ref(a)
    print(r())
    a = None
    clean_the_stack = [0, 0, 0, 0]
    gc.collect()
    print(r())

    print("test use of ref() with a callback")
    a = A()
    r = weakref.ref(a, callback)
    print(r())
    a = None
    clean_the_stack = [0, 0, 0, 0]
    gc.collect()
    print(r())

    print("test when weakref gets collected before the object it refs")
    a = A()
    r = weakref.ref(a, callback)
    print(r())
    r = None
    clean_the_stack = [0, 0, 0, 0]
    gc.collect()
    a = None

    print("test a double reference")
    a = A()
    r1 = weakref.ref(a, callback)
    r2 = weakref.ref(a, callback)
    print(r1(), r2())
    a = None
    clean_the_stack = [0, 0, 0, 0]
    gc.collect()
    print(r1(), r2())


test()
