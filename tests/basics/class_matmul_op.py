# test __matmul__ binary operator

class A:
    def __init__(self, v):
        self.v = v

    def __matmul__(self, o):
        return A(self.v + o.v)

    def __repr__(self):
        return "A(%s)" % self.v

a = A(5)
b = A(3)
c = a @ b

print(a)
print(b)
print(c)

# __imatmul__ not be defined, will be emulated using __matmul__
a @= b

print(a)
print(b)
print(c)

# Same again for __rmatmul__
class B:
    def __init__(self, v):
        self.v = v

    def __matmul__(self, o):
        return B(self.v + o)

    def __rmatmul__(self, o):
        return B(self.v + o)

    def __repr__(self):
        return "B(%s)" % self.v

a = B(5)
b = 3
c = 7
d = a @ b
e = c @ a

print(a)
print(b)
print(c)
print(d)
print(e)
