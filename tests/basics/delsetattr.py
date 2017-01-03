# test __delattr__ and __setattr__

class A:
    def __init__(self, d):
        A.d = d

    def __getattr__(self, attr):
        if attr in A.d:
            return A.d[attr]
        else:
            raise AttributeError(attr)

    def __setattr__(self, attr, value):
        A.d[attr] = value

    def __delattr__(self, attr):
        del A.d[attr]

a = A({"a":1, "b":2})
print(a.a, a.b)
a.a = 3
try:
    print(a.a, a.b)
    del a.a
    print(a.a)
except AttributeError:
    print("AttributeError")
