# Test reverse operators.

# Test user type with integers.
class A:
    def __init__(self, v):
        self.v = v

    def __add__(self, o):
        if isinstance(o, A):
            return A(self.v + o.v)
        return A(self.v + o)

    def __radd__(self, o):
        return A(self.v + o)

    def __repr__(self):
        return "A({})".format(self.v)


print(A(3) + 1)
print(2 + A(5))


# Test user type with strings.
class B:
    def __init__(self, v):
        self.v = v

    def __repr__(self):
        return "B({})".format(self.v)

    def __ror__(self, o):
        return B(o + "|" + self.v)

    def __radd__(self, o):
        return B(o + "+" + self.v)

    def __rmul__(self, o):
        return B(o + "*" + self.v)

    def __rtruediv__(self, o):
        return B(o + "/" + self.v)


print("a" | B("b"))
print("a" + B("b"))
print("a" * B("b"))
print("a" / B("b"))

x = "a"; x |= B("b"); print(x)
x = "a"; x += B("b"); print(x)
x = "a"; x *= B("b"); print(x)
x = "a"; x /= B("b"); print(x)
