# Test constant optimisation, with full range of const types.
# This test will only work when MICROPY_COMP_CONST and MICROPY_COMP_CONST_TUPLE are enabled.

from micropython import const

_INT = const(123)
_STR = const("str")
_BYTES = const(b"bytes")
_TUPLE = const((_INT, _STR, _BYTES))
_TUPLE2 = const((None, False, True, ..., (), _TUPLE))

print(_INT)
print(_STR)
print(_BYTES)
print(_TUPLE)
print(_TUPLE2)

x = _TUPLE
print(x is _TUPLE)
print(x is (_INT, _STR, _BYTES))

print(hasattr(globals(), "_INT"))
print(hasattr(globals(), "_STR"))
print(hasattr(globals(), "_BYTES"))
print(hasattr(globals(), "_TUPLE"))
print(hasattr(globals(), "_TUPLE2"))
