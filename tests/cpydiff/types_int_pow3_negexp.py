"""
categories: Types,int
description: 3-argument ``pow()`` with a negative exponent returns ``0``.
cause: MicroPython returns ``0`` for any negative exponent instead of computing the
modular inverse of the base. Builds not configured with ``MICROPY_LONGINT_IMPL_MPZ``
return a float instead.
workaround: Compute the modular inverse explicitly, e.g. with the extended Euclidean algorithm.
"""

# invertible base
print(pow(2, -1, 5))

# non-invertible base
try:
    print(pow(2, -1, 4))
except ValueError:
    print("ValueError")
