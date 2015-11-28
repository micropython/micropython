class A:

    var = 132

    def __init__(self):
        self.var2 = 34

    def meth(self, i):
        return 42 + i


a = A()
print(hasattr(a, "var"))
print(hasattr(a, "var2"))
print(hasattr(a, "meth"))
print(hasattr(a, "_none_such"))
print(hasattr(list, "foo"))

class C:

    def __getattr__(self, attr):
        if attr == "exists":
            return attr
        raise AttributeError

c = C()
print(hasattr(c, "exists"))
# TODO
#print(hasattr(c, "doesnt_exist"))

try:
    hasattr(1, b'123')
except TypeError:
    print('TypeError')

try:
    hasattr(1, 123)
except TypeError:
    print('TypeError')
