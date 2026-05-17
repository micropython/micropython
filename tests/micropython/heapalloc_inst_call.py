# Test that calling clazz.__call__() with up to at least 3 arguments
# doesn't require heap allocation.

try:
    import micropython

    micropython.heap_lock
except (ImportError, AttributeError):
    print("SKIP")
    raise SystemExit


class Foo0:
    def __call__(self):
        print("__call__")


class Foo1:
    def __call__(self, a):
        print("__call__", a)


class Foo2:
    def __call__(self, a, b):
        print("__call__", a, b)


class Foo3:
    def __call__(self, a, b, c):
        print("__call__", a, b, c)


f0 = Foo0()
f1 = Foo1()
f2 = Foo2()
f3 = Foo3()

micropython.heap_lock()
f0()
f1(1)
f2(1, 2)
f3(1, 2, 3)
micropython.heap_unlock()
