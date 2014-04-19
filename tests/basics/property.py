class A:
    def __init__(self, x):
        self._x = x

    @property
    def x(self):
        print("x get")
        return self._x

a = A(1)
print(a.x)

try:
    a.x = 2
except AttributeError:
    print("AttributeError")

class B:
    def __init__(self, x):
        self._x = x

    def xget(self):
        print("x get")
        return self._x

    def xset(self, value):
        print("x set")
        self._x = value

    x = property(xget, xset)

b = B(3)
print(b.x)
b.x = 4
print(b.x)

class C:
    def __init__(self, x):
        self._x = x

    @property
    def x(self):
        print("x get")
        return self._x

    @x.setter
    def x(self, value):
        print("x set")
        self._x = value

c = C(5)
print(c.x)
c.x = 6
print(c.x)
