# test __delattr__ and __setattr__

# feature test for __setattr__/__delattr__
try:
    class Test():
        def __delattr__(self, attr): pass
    del Test().noexist
except AttributeError:
    print('SKIP')
    raise SystemExit

# this class just prints the calls to see if they were executed
class A():
    def __getattr__(self, attr):
        print('get', attr)
        return 1
    def __setattr__(self, attr, val):
        print('set', attr, val)
    def __delattr__(self, attr):
        print('del', attr)
a = A()

# check basic behaviour
print(getattr(a, 'foo'))
setattr(a, 'bar', 2)
delattr(a, 'baz')

# check meta behaviour
getattr(a, '__getattr__') # should not call A.__getattr__
getattr(a, '__setattr__') # should not call A.__getattr__
getattr(a, '__delattr__') # should not call A.__getattr__
setattr(a, '__setattr__', 1) # should call A.__setattr__
delattr(a, '__delattr__') # should call A.__delattr__

# this class acts like a dictionary
class B:
    def __init__(self, d):
        # store the dict in the class, not instance, so
        # we don't get infinite recursion in __getattr_
        B.d = d

    def __getattr__(self, attr):
        if attr in B.d:
            return B.d[attr]
        else:
            raise AttributeError(attr)

    def __setattr__(self, attr, value):
        B.d[attr] = value

    def __delattr__(self, attr):
        del B.d[attr]

a = B({"a":1, "b":2})
print(a.a, a.b)
a.a = 3
print(a.a, a.b)
del a.a
try:
    print(a.a)
except AttributeError:
    print("AttributeError")

# test object.__setattr__
class C:
    def __init__(self):
        pass

    def __setattr__(self, attr, value):
        print(attr, "=", value)

    def __delattr__(self, attr):
        print("del", attr)

c = C()
c.a = 5
try:
    print(c.a)
except AttributeError:
    print("AttributeError")

object.__setattr__(c, "a", 5)
super(C, c).__setattr__("b", 6)
print(c.a)
print(c.b)

try:
    # attribute name must be string
    object.__setattr__(c, 5, 5)
except TypeError:
    print("TypeError")


# test object.__delattr__
del c.a
print(c.a)

object.__delattr__(c, "a")
try:
    print(c.a)
except AttributeError:
    print("AttributeError")

super(C, c).__delattr__("b")
try:
    print(c.b)
except AttributeError:
    print("AttributeError")

try:
    object.__delattr__(c, "c")
except AttributeError:
    print("AttributeError")
