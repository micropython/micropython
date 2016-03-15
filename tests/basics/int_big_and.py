print(0 & (1 << 80))
print(0 & (1 << 80) == 0)
print(bool(0 & (1 << 80)))

a = 0xfffffffffffffffffffffffffffff
print(a & (1 << 80))
print((a & (1 << 80)) >> 80)
print((a & (1 << 80)) >> 80 == 1)

# test negative on rhs
a = 123456789012345678901234567890
print(a & -1)
print(a & -2)
print(a & -2345678901234567890123456789)
print(a & (-a))
print(a & (-a - 1))
print(a & (-a + 1))

# test negative on lhs
a = 123456789012345678901234567890
print(-1 & a)
print(-2 & a)
print(-2345678901234567890123456789 & a)
print((-a) & a)
print((-a) & 0xffffffff)
print((-a) & 0xffffffffffffffffffffffffffffffff)
print((-a) & 2)
print((-(1 << 70)) & 2)

# test negative on lhs and rhs
mpz = 1 << 70
a = 123456789012345678901234567890
print(-1 & (-a))
print(-2 & (-a))
print(-2345678901234567890123456789 & (-a))
print((-a) & (-a))
print((-a) & (-0xffffffff))
print((-a) & (-0xffffffffffffffffffffffffffffffff))
print((-1) & (-0xffffffffffffffffffffffffffffffff))
print((-a) & (-2))
print((-mpz) & (-2))
