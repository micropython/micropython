# test constant optimisation, with consts that are bignums

from micropython import const

# check we can make consts from bignums
Z1 = const(0xffffffff)
Z2 = const(0xffffffffffffffff)
print(hex(Z1), hex(Z2))

# check arithmetic with bignum
Z3 = const(Z1 + Z2)
Z4 = const((1 << 100) + Z1)
print(hex(Z3), hex(Z4))
