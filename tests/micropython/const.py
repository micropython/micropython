# test constant optimisation
# This test will only work when MICROPY_COMP_CONST is enabled.

from micropython import const

X = const(123)
Y = const(X + 456)

print(X, Y + 1)


def f():
    print(X, Y + 1)


f()

_X = const(12)
_Y = const(_X + 34)

print(_X, _Y)


class A:
    Z = const(1)
    _Z = const(2)
    print(Z, _Z)


print(hasattr(A, "Z"), hasattr(A, "_Z"))
