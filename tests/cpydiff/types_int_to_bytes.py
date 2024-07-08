"""
categories: Types,int
description: ``to_bytes`` method doesn't implement signed parameter.
cause: The ``signed`` keyword-only parameter is not implemented for ``int.to_bytes()``.

When the integer is negative, MicroPython behaves the same as CPython ``int.to_bytes(..., signed=True)``

When the integer is non-negative, MicroPython behaves the same as CPython ``int.to_bytes(..., signed=False)``.

(The difference is subtle, but in CPython a positive integer converted with ``signed=True`` may require one byte more in the output length, in order to fit the 0 sign bit.)

workaround: Take care when calling ``to_bytes()`` on an integer value which may be negative.
"""

x = -1
print(x.to_bytes(1, "big"))
