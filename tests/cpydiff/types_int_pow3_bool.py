"""
categories: Types,int
description: 3-argument ``pow()`` rejects ``bool`` operands.
cause: On builds configured with ``MICROPY_LONGINT_IMPL_MPZ`` (the default
for unix and most ports), the 3-argument ``pow()`` requires its operands to
pass a strict integer type check, which excludes ``bool``.
workaround: Convert ``bool`` operands to ``int`` explicitly, e.g. ``pow(int(b), y, m)``.
"""

# bool in base position
try:
    print(pow(True, 1, 1))
except TypeError as e:
    print("TypeError")

# bool in exponent position
try:
    print(pow(1, True, 1))
except TypeError as e:
    print("TypeError")

# bool in modulus position
try:
    print(pow(1, 1, True))
except TypeError as e:
    print("TypeError")
