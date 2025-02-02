# test subclassing a native type and overriding __init__

# overriding list.__init__()
class L(list):
    def __init__(self, a, b):
        super().__init__([a, b])
print(L(2, 3))

# with keyword arguments, with star arguments and without because those use different C calls
class D(dict):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
print(D())
print(D([('a', 1)]))
print(D([('a', 1)], a=2, b=3))
print(D(a=2, b=3))

class D(dict):
    def __init__(self):
        super().__init__()
print(D())

class D(dict):
    def __init__(self):
        super().__init__([])
print(D())

class D(dict):
    def __init__(self):
        super().__init__(a=1)
print(D())

class D(dict):
    def __init__(self):
        super().__init__([], a=1)
print(D())

# inherits implicitly from object
class A:
    def __init__(self):
        print("A.__init__")
        super().__init__()
A()

# inherits explicitly from object
class B(object):
    def __init__(self):
        print("B.__init__")
        super().__init__()
B()

# multiple inheritance with object explicitly mentioned
class C:
    pass
class D(C, object):
    def __init__(self):
        print('D.__init__')
        super().__init__()
    def reinit(self):
        print('D.foo')
        super().__init__()
a = D()
a.__init__()
a.reinit()

# call __init__() after object is already init'd
class L(list):
    def reinit(self):
        super().__init__(range(2))
a = L(range(5))
print(a)
a.reinit()
print(a)
