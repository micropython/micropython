# Test type annotations in combination with const.
# This test will only work when MICROPY_COMP_CONST and MICROPY_COMP_CONST_TUPLE are enabled.

from micropython import const

_X0: bool = const(True)
_X1: int = const(123)
_X2: str = const("test")
_X3: tuple = const((1, 2))
_X4: tuple[bool, int] = const((True, 4))
_X5: bytes = b"\x01\x02\x03"

print(_X0)
print(_X1)
print(_X2)
print(_X3)
print(_X4)
print(_X5)
