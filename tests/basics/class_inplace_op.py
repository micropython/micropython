# Case 1: Immutable object (e.g. number-like)
# __iadd__ should not be defined, will be emulated using __add__

class A:

    def __init__(self, v):
        self.v = v

    def __add__(self, o):
        return A(self.v + o.v)

    def __repr__(self):
        return "A({})".format(self.v)

a = A(5)
b = a
a += A(3)
print(a)
# Should be original a's value, i.e. A(5)
print(b)

# Case 2: Mutable object (e.g. list-like)
# __iadd__ should be defined

class L:

    def __init__(self, v):
        self.v = v

    def __add__(self, o):
        # Should not be caled in this test
        print("L.__add__")
        return L(self.v + o.v)

    def __iadd__(self, o):
        self.v += o.v
        return self

    def __repr__(self):
        return "L({})".format(self.v)

c = L([1, 2])
d = c
c += L([3, 4])
print(c)
# Should be updated c's value, i.e. L([1, 2, 3, 4])
print(d)
