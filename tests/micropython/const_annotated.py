# test constant optimisation
# This test will only work when MICROPY_COMP_CONST is enabled.

from micropython import const

X: int = const(123)
Y: int = const(X + 456)

print(X, Y + 1)


def f():
    print(X, Y + 1)


f()

_X: int = const(12)
_Y: int = const(_X + 34)

print(_X, _Y)


class A:
    Z: int = const(1)
    _Z: int = const(2)
    print(Z, _Z)


print(hasattr(A, "Z"), hasattr(A, "_Z"))
