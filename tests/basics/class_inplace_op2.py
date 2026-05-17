# Test inplace special methods enabled by MICROPY_PY_ALL_INPLACE_SPECIAL_METHODS


class A:
    def __imul__(self, other):
        print("__imul__")
        return self

    def __imatmul__(self, other):
        print("__imatmul__")
        return self

    def __ifloordiv__(self, other):
        print("__ifloordiv__")
        return self

    def __itruediv__(self, other):
        print("__itruediv__")
        return self

    def __imod__(self, other):
        print("__imod__")
        return self

    def __ipow__(self, other):
        print("__ipow__")
        return self

    def __ior__(self, other):
        print("__ior__")
        return self

    def __ixor__(self, other):
        print("__ixor__")
        return self

    def __iand__(self, other):
        print("__iand__")
        return self

    def __ilshift__(self, other):
        print("__ilshift__")
        return self

    def __irshift__(self, other):
        print("__irshift__")
        return self


a = A()

try:
    a *= None
except TypeError:
    print("SKIP")
    raise SystemExit

a @= None
a //= None
a /= None
a %= None
a **= None
a |= None
a ^= None
a &= None
a <<= None
a >>= None

# Normal operator should not fallback to inplace operator
try:
    a * None
except TypeError:
    print("TypeError")
