import skip_if
skip_if.no_reverse_ops()

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
