# Test specifying a metaclass via the `metaclass=` keyword in a class definition.
#
# Note: this test avoids accessing `__name__` on classes because it's only
# available when MICROPY_CPYTHON_COMPAT is enabled (not in minimal builds).


class MyMeta(type):
    def __new__(meta, name, bases, dct):
        print("MyMeta.__new__", name, len(bases))
        if hasattr(type, "__new__"):
            # CPython
            return type.__new__(meta, name, bases, dct)
        else:
            # MicroPython: type has no __new__ attribute, so build the class
            # with type() directly.
            return type(name, bases, dct)

    def __init__(cls, name, bases, dct):
        print("MyMeta.__init__", name, len(bases))
        super().__init__(name, bases, dct)


class B:
    def __init__(self):
        print("B.__init__")


# metaclass with no bases
class A(metaclass=MyMeta):
    def __init__(self):
        print("A.__init__")


# metaclass with an explicit base
class C(B, metaclass=MyMeta):
    def __init__(self):
        print("C.__init__")
        super().__init__()


# ordinary classes are unaffected
class D(B):
    def __init__(self):
        print("D.__init__")
        super().__init__()


# instances of classes created by the metaclass work normally
for cls in (A, B, C, D):
    obj = cls()
    print(type(obj) is cls, isinstance(obj, B), repr(obj).startswith("<"))
