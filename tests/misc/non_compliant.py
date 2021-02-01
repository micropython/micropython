# tests for things that are not implemented, or have non-compliant behaviour

try:
    import uarray as array
    import ustruct
except ImportError:
    print("SKIP")
    raise SystemExit

# when super can't find self
try:
    exec("def f(): super()")
except SyntaxError:
    print("SyntaxError")

# store to exception attribute is not allowed
try:
    ValueError().x = 0
except AttributeError:
    print("AttributeError")

# array deletion not implemented
try:
    a = array.array("b", (1, 2, 3))
    del a[1]
except TypeError:
    print("TypeError")

# slice with step!=1 not implemented
try:
    a = array.array("b", (1, 2, 3))
    print(a[3:2:2])
except NotImplementedError:
    print("NotImplementedError")

# containment, looking for integer not implemented
try:
    print(1 in array.array("B", b"12"))
except NotImplementedError:
    print("NotImplementedError")

# uPy raises TypeError, shold be ValueError
try:
    "%c" % b"\x01\x02"
except (TypeError, ValueError):
    print("TypeError, ValueError")

# attributes/subscr not implemented
try:
    print("{a[0]}".format(a=[1, 2]))
except NotImplementedError:
    print("NotImplementedError")

# str(...) with keywords not implemented
try:
    str(b"abc", encoding="utf8")
except NotImplementedError:
    print("NotImplementedError")

# str.rsplit(None, n) not implemented
try:
    "a a a".rsplit(None, 1)
except NotImplementedError:
    print("NotImplementedError")

# str.endswith(s, start) not implemented
try:
    "abc".endswith("c", 1)
except NotImplementedError:
    print("NotImplementedError")

# str subscr with step!=1 not implemented
try:
    print("abc"[1:2:3])
except NotImplementedError:
    print("NotImplementedError")

# bytes(...) with keywords not implemented
try:
    bytes("abc", encoding="utf8")
except NotImplementedError:
    print("NotImplementedError")

# bytes subscr with step!=1 not implemented
try:
    b"123"[0:3:2]
except NotImplementedError:
    print("NotImplementedError")

# tuple load with step!=1 not implemented
try:
    ()[2:3:4]
except NotImplementedError:
    print("NotImplementedError")

# list store with step!=1 not implemented
try:
    [][2:3:4] = []
except NotImplementedError:
    print("NotImplementedError")

# list delete with step!=1 not implemented
try:
    del [][2:3:4]
except NotImplementedError:
    print("NotImplementedError")

# struct pack with too many args, not checked by uPy
print(ustruct.pack("bb", 1, 2, 3))

# struct pack with too few args, not checked by uPy
print(ustruct.pack("bb", 1))

# array slice assignment with unsupported RHS
try:
    bytearray(4)[0:1] = [1, 2]
except NotImplementedError:
    print("NotImplementedError")

# can't assign attributes to a function
def f():
    pass


try:
    f.x = 1
except AttributeError:
    print("AttributeError")

# can't call a function type (ie make new instances of a function)
try:
    type(f)()
except TypeError:
    print("TypeError")

# test when object explicitly listed at not-last position in parent tuple
# this is not compliant with CPython because of illegal MRO
class A:
    def foo(self):
        print("A.foo")


class B(object, A):
    pass


B().foo()

# can't assign property (or other special accessors) to already-subclassed class
class A:
    pass


class B(A):
    pass


try:
    A.bar = property()
except AttributeError:
    print("AttributeError")
