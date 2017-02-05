
class A:

    def __init__(self, a=0, b=0, c=0, d=b''):
        self._a = a
        self._b = b
        self._c = c
        self._d = d

    def __getattr__(self, name):
        if not name.startswith("_"):
            name = "_" + name
        return object.__getattribute__(self, name)

    def __setattr__(self, name, value):
        if not name.startswith("_"):
            name = "_" + name
        object.__setattr__(self, name, value)

    def __delattr__(self, name):
        if not name.startswith("_"):
            name = "_" + name
        object.__delattr__(self, name)


a = A(1, 2, 3, b'value')
a.e = 4
print(a.e)
print(a.b)
try:
    del a.c
    print(a.c)
except AttributeError as ae:
    print("AttributeError", ae)
