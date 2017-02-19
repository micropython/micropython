# test __delattr__ and __setattr__

# feature test for __setattr__/__delattr__
try:
    class Test():
        def __delattr__(self, attr): pass
    del Test().noexist
except AttributeError:
    import sys
    print('SKIP')
    sys.exit()

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
